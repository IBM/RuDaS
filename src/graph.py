#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Sun Sep 16 00:56:04 2018

@author: veronika.thost
"""

import random
import copy
import logic as log

def remove_duplicates_in_values(d):
    for k in d:
        d[k] = remove_duplicates(d[k])
    return d

#list(set()) does somehow not work with our classes
def remove_duplicates(l):
    l1 = []
    for e in l:
        if e not in l1:
            l1.append(e)
    return l1

class Node(object):
    def __init__(self, nid, parents):
        self.nid = nid
        self.parents = parents
        self.parent = None
        self.children = []
        

class RuleNode(Node):
    def __init__(self, nid, parents, rule):
        super(RuleNode,self).__init__(nid, parents)
        self.rule = rule


class OrNode(Node):
    def __init__(self, nid, parents):
        super(OrNode,self).__init__(nid, parents)
        
#use generate_rulenode to generate rule nodes
#otherwise related graph attributes have to be updated manually
#update(...) has to be called though
#target must be in predicates1
class InferenceStructure(object):
    
    def __init__(self, target, predicates1, constants1, max_atoms1, max_depth, max_occ, recursive, max_ordegree):
        global constants
        global predicates
        global max_atoms
    
        constants = constants1
        predicates = predicates1
        max_atoms = max_atoms1

        self.max_depth = max_depth
        self.max_occ = max_occ
        self.max_ordegree = max_ordegree
        self.recursive = recursive
        #currently we only keep the rule nodes in nodes, in leaves anyway only rule nodes
        self.nodes = [] 
        self.leaves = []
        #nid and variables are for tracking used ones
        self.next_node_id = 0
        self.variables = []
        #map head variable names (of child nodes with several parents)
        #to list containing corresponding variables in parents atoms
        self.union_variables = {}
        self.const_variables = {}
        #all body atoms with corresponding nodes occurring in some rule in the graph
        self.body_atoms = {p.name:[] for p in predicates}
        # name of target predicate
        self.target = target
        #is specific to kind of graph 
        #(e.g., sometimes only contains predicates from rule heads)
        self.used_predicates = {}
# -----------------------------------------------
#TODO also allow some constants (but probably require at least one variable?)
        args = [self.get_new_var() for _ in range(target.arity)]
        self.root = self.generate_rulenode(log.Atom(target, args), None)
        self.update([self.root]) #updates nodes and leaves

        for i in range(max_depth-1): #root rule is already one step
            if i > 0 or not isinstance(self,RootedDAG):
                self.extend()
            else:
                self.extend(specialnode=True)

#        print(atoms_tostr(self.body_atoms)) #"".join([atoms_tostr(v) for v in self.body_atoms.values()]))
            
    def __str__(self):
        return self.node_tostr(self.root, "", [])
    
    def node_tostr(self, node, offset, processed):
        processed.append(node.nid)
        s = "["+ str(node.nid) + "] "
        if isinstance(node, RuleNode):
            s += str(node.rule) 
        else:
            s += "OR"
          
        cs = [self.node_tostr(c, offset+'\t', processed) if c.nid not in processed \
              else offset+"\t|- "+"["+ str(c.nid) + "] " for c in node.children ]
            
        if cs:
            return offset+"|- "+s+"\n"+"\n".join(cs)
        
        return offset+"|- "+s


    # def get_all_nodes(self):
    #     processed = [self.root.nid]
    #     ns = [self.root]
    #     cs = [self.root]
    #     cstmp = []
    #     while cs:
    #         for c in cs:
    #             if c.nid not in processed:
    #                 processed.append(c.nid)
    #                 ns.append(c)
    #                 cstmp.extend(c.children)
    #
    #         cs = cstmp
    #     return ns

    def contains_ornodes(self):
        processed = [self.root.nid]
        cs = self.root.children
        cstmp = []
        while cs:
            for c in cs:
                if isinstance(c,OrNode):
                    return True
                if c.nid not in processed:
                    processed.append(c.nid)
                    cstmp.extend(c.children)

            cs = cstmp
        return False

    def get_relevant_children(self, node):
        return [c for c in node.children if c.parent.nid == node.nid]
        
# get all rules in graph        
    def get_rules(self):
        return [n.rule for n in self.nodes if isinstance(n, RuleNode)]
    
    def get_alternative_leaves(self, node):
        cs = self.get_relevant_children(node)
        if not cs:
#            print('no children ' + str(node.nid))
            return [[node]]
        
        l = [l for c in cs for l in self.get_alternative_leaves(c)]
        
        if isinstance(node, RuleNode):
            return l
        #flatten
        return [[n for l1 in l for n in l1]]
     
    def generate_ornode(self, head, parent):
        
        body_atoms = self.body_atoms[head.predicate.name]
        # if len(body_atoms) > 1:
            #print("multi "+str(head.predicate))
            #TODO support this case (with variable renaming?)
        
        parents = remove_duplicates([n for _,n in body_atoms])
        
        n = OrNode(self.get_new_node_id(), parents) #[parent]
        n.parent = parent
        for parent in parents:
            parent.children.append(n)
        
        for _ in range(random.randint(2, max (2, self.max_ordegree))):
            self.generate_rulenode(head, n)
        
        if n.children:
            return n
        return None
    
    def generate_rulenode(self, head, parent):
        if head is None:
            return None
        
        body_atoms = self.body_atoms[head.predicate.name]
        # if len(body_atoms) > 1: #TODO might need later put the entire head in some dict eg for printing? no can check several parents?
            #print("multi "+str(head.predicate))
            #TODO support this case (with variable renaming?)
#            TODO problematic with unifier creation solve later
#            args = []
#            for i, t in enumerate(head.arguments):
#                if isinstance(t,Variable):
#                    v = self.get_new_var()
#                    self.union_variables[v.name] = [a.arguments[i] for a,_ in body_atoms if isinstance(a.arguments[i], Variable)]
#                    #TODO latter else must later also update newly introduced var-constant dict
#                    args.append(v)
#                else:
#                    args.append(t)
#                    
#            head = Atom(head.predicate, args) 
        
        r = self.generate_rule(head)
        if r is not None:
            parents = remove_duplicates([n for _,n in body_atoms]) if not isinstance(parent, OrNode) else [parent]#[parent] if parent is not None else []
            n = RuleNode(self.get_new_node_id(), parents, r)
            if n is not None:
                n.parent = parent
#                if parent is not None:
                for parent in parents:
                    parent.children.append(n) #TODO check if this can be set in init of child
                #graph attribute updates
                for a in n.rule.body:
                    self.body_atoms[a.predicate.name].append((a,n))
                self.update_used_predicates(self.get_used_predicates(n.rule))   
            return n
        return None
    
    def generate_rule(self, head):
        #TODO we could also replace head variables by constants but this is not so very helpful... ?
        head = copy.deepcopy(head)
        for i, t in enumerate(head.arguments):
            if isinstance(t, log.Constant): # currently no constants in head
#                print(strlist(head.arguments))
                v = self.get_new_var()
                head.arguments.remove(t)
                head.arguments.insert(i,v)
#                print(strlist(head.arguments))
                self.const_variables[v.name] = t
        
        hvs = remove_duplicates([v for v in head.arguments if isinstance(v, log.Variable)]) #TODO check if set works
          
        #in our case since maxarity (bound on head vars)=maxatoms the following works
        #get head variable positions
        ps = [(i,self.get_predicate(head.predicate.name)) for i in range(max_atoms)]
        hvps = [(i,p.name,j) for i,p in ps if p is not None for j in range(p.arity)]
        #not enough positions since not enough predicates available anymore
        if len(hvps) < len(hvs):
            print('!! stopping generation at: '+str(head))
            return None
        
        random.shuffle(hvps)
        hvps = hvps[0:len(hvs)]
        #print(str(head)+str( hvps))
        ps1 = [i for i,p,j in hvps]
        ps = [(i,p) for i,p in ps if i in ps1 or random.randint(0,2)==0]
        
        allvars = []
        body = []
        for j,p in ps:
            args = [ hvs[hvps.index((j,p.name, i))] if (j,p.name, i) in hvps 
                     else hvs[random.randint(0, len(hvs)-1)] if random.randint(0, 4) == 0
                     else allvars[random.randint(0, len(allvars)-1)] if allvars != [] and random.randint(0, 3) > 0
                     #else constants[random.randint(0, len(constants)-1)] if random.randint(0, 9) == 0
                     else self.get_new_var() 
                     for i in range(p.arity)]
            allvars.extend([v for v in args if v not in allvars])
            atom = log.Atom(p, args)
            if atom not in body:
                body.append(atom)

        return log.Rule(remove_duplicates(body), head) #TODO CHECK the set transformation does it delete duplicates?
     
    def get_predicate(self, head_predicate):
        l = [i for i in range(len(predicates))]
        random.shuffle(l)       
        for i in range(len(predicates)):
            p = predicates[l[i]]
            if p.name != self.target.name and \
            (p.name not in self.used_predicates or \
             self.used_predicates[p.name] < self.max_occ) and \
            (p.name != head_predicate or self.recursive):#TODO on leave level we could allow some more
                return p
        return None     
    
    def get_new_node_id(self):
        self.next_node_id += 1
        return self.next_node_id - 1
    
    def get_new_var(self):
        v = log.Variable(len(self.variables))
        self.variables.append(v)
        return v 
    
    def update(self, nodes):
        if not nodes: #occurs if no body atoms could be selected anymore to be new heads
            return
        #possible result of generate_rulenode      
        nodes = [n for n in nodes if n is not None]
        #I assume we extend a tree simultaneously (vs. in one path) 
        #such that cs contains ALL new leaves
        self.leaves = []
        for c in nodes:
            if isinstance(c, RuleNode):
                self.nodes.append(c)
                self.leaves.append(c)
            else:
                #self.nodes.append(c)
                self.nodes.extend(c.children)
                self.leaves.extend(c.children) #must be RuleNodes
                
    def update_used_predicates(self, predicates):
        for p in predicates:
            if p.name not in self.used_predicates:
                self.used_predicates[p.name] = 1
            else:
                self.used_predicates[p.name] = self.used_predicates[p.name] + 1
#            print(str(p.name)+" "+str(self.used_predicates[p.name]))
             
    #check if an atom can serve as head atom  
    #assumes that atom is a body atom in rule           
    def check_atom(self, atom, rule):
        #TODO could discuss that, it could serve as head, but the constant replacement is randomly
        #so it would not be sure that they are replaced. 
        #and still then the derivation would be boring because the variable's domain contains only the one constant
        #TODO or maybe accept an only constant atom here if there are no others in rule
        if len([c for c in atom.arguments if isinstance(c, log.Constant)]) == len(atom.arguments):
            return False

        hvs = remove_duplicates([v.name for v in rule.head.arguments if isinstance(v, log.Variable)])
        avs = remove_duplicates([v.name for v in atom.arguments if isinstance(v, log.Variable)])
        #check if there is some connection
        for a in rule.body:
            hv = [1 for t in a.arguments if isinstance(t, log.Variable) and t.name in hvs]
            av = [1 for t in a.arguments if isinstance(t, log.Variable) and t.name in avs]
            if hv and av:
                return True
        #print('check failed ' + str(atom))
        return False   

class ChainDAG(InferenceStructure):
    
    def __init__(self, target, predicates, constants, max_atoms, max_depth, recursive):
        super(ChainDAG,self).__init__(target, predicates, constants, max_atoms, max_depth, 1, recursive, 1)
 
    def extend(self):
        for a in self.get_random_atoms(self.leaves[0].rule):
            n = super(ChainDAG,self).generate_rulenode(a, self.leaves[0])
            if n is not None:
                super(ChainDAG,self).update([n])
                return
        
    def get_used_predicates(self, rule): 
        return rule.get_predicates()
    
#TODO below we get no atoms if all have only constants, disallow this in rule generation explicitly?
    def get_random_atoms(self, rule):
        l = [a for a in rule.body if super(ChainDAG,self).check_atom(a, rule)]
        random.shuffle(l)
        return l #l[0] if l else None

# (is not relevant in chains because there any body predicate once ocurring is "used")
# and especially add as children also of eg previous predicate occurrences in tree
# in disjunctive trees we don't have to ignore them but then replace the 
# correponding body atom's children by OR nodes       
class RootedDAG(InferenceStructure):
    
    def __init__(self, target, predicates, constants, max_atoms, max_depth, recursive, max_ordegree):
        super(RootedDAG,self).__init__(target, predicates, constants, max_atoms, max_depth, 1, recursive, max_ordegree)

    # specialnode to mark when as first child create several or or node depending on category
    # to ensure we get dataset of requested category
    def extend(self, specialnode=False):
        nodes = []
        nostop = random.randint(0,len(self.leaves)-1)
        for j, l in enumerate(self.leaves):
            # if j != nostop and random.randint(0,3) == 0:
            #     print('stop at', l.rule)
            #     continue
            atoms = self.get_random_atoms(l.rule)
            # generate only a single child with probability 20%
            m = 1 if len(atoms) <= 1 or (not specialnode and random.randint(0, 4) == 0) else random.randint(2, len(atoms))
            i = 0
            #possible atom indices
            for a in atoms:
                if self.max_ordegree == 1 or random.randint(0, 1) == 0:#(not specialnode and random.randint(0, 1) == 0):
                    n = super(RootedDAG,self).generate_rulenode(a, l)      
                else:
                    n = super(RootedDAG,self).generate_ornode(a, l)
                if n is not None:
                    i += 1
                    nodes.append(n)
                    if i == m:
                        break
                
        super(RootedDAG,self).update(nodes)

    def get_used_predicates(self, rule): #node is a rulenode
        return [rule.head.predicate]
#TODO below we get no atoms if all have only constants, maybe disallow this in rule generation explicitly
    def get_random_atoms(self, rule):
        l = [a for a in rule.body if a.predicate.name not in self.used_predicates and \
             super(RootedDAG,self).check_atom(a, rule)]
        random.shuffle(l)
#        print(strlist(l))
        return l #if len(l) < 2 else l[0:random.randint(1, len(l))]
    

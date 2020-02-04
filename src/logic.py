'''
 Copyright IBM Corp. 1992, 1993 All Rights Reserved
SPDX-License-Identifier: Apache-2.0
'''

import copy
import sys
import itertools

def count_facts_dict(dict):
    num_facts = 0
    for key, value in dict.items():
        num_facts += len([item for item in value if item])
    return num_facts


def union_dict(dict1, dict2):
    keys = set(dict1).union(dict2)
    union = {}
    for key in keys:
        list_d = []
        list1 = []
        list2 = []
        if key in dict1:
            list1 = dict1[key]
        if key in dict2:
            list2 = dict2[key]
        for element in list1:
            list_d.append(element)
        for element in list2:
            if element not in list_d:
                list_d.append(element)
        union[key] = list_d
    return union


def delta_dict(dict1, dict2):
    keys = set(dict1).union(dict2)
    delta = {}
    for key in keys:
        list_d = []
        list1 = []
        list2 = []
        if key in dict1:
            list1 = dict1[key]
        if key in dict2:
            list2 = dict2[key]
        for element in list1:
            if element not in list2:
                list_d.append(element)
        for element in list2:
            if element not in list1:
                list_d.append(element)
        delta[key] = list_d
    return delta

class LogicProgram(object):
    # we assume the num of predicates and constants are from 0 to num_predicates/num_constants
    def __init__(self, facts={}, rules=[], num_predicates=-1, num_constants=-1, grounded=False):
        self.num_predicates = num_predicates
        self.num_constants = num_constants
        self.rules = rules
        self.original_facts = facts
        # self.grounded_facts = []
        self.inferred_facts = {}
        self.grounded = grounded
        if rules == []:
            # self.grounded_facts = copy.deepcopy(self.original_facts)
            self.grounded = True
        self.num_original_facts = 0
        self.num_original_facts = count_facts_dict(self.original_facts)
        self.num_grounded_facts = None  # currently not used
        self.predicates_list = []


    def add_inferred_facts(self, inferred_facts):
        self.inferred_facts = copy.deepcopy(inferred_facts)
        self.grounded = True

    def add_facts(self, facts):
        self.original_facts = union_dict(self.original_facts, facts)

    def ground_program(self):
        if not self.grounded:
            if self.rules != []:
                grounded_facts = forward_inference(copy.deepcopy(self.original_facts), self.rules)
                # extract inferred facts
                self.inferred_facts = delta_dict(grounded_facts, self.original_facts)
                self.num_grounded_facts = count_facts_dict(grounded_facts)
            self.grounded = True

    def return_groundings(self):
        grounded_facts = union_dict(self.original_facts, self.inferred_facts)
        return grounded_facts


class Rule(object):
    '''
    Parameters
    ----------
    body   :   list of atoms
        body atoms
    head    :   atom
        head atom
    '''
    def __init__(self, body, head):
        self.body = body
        self.head = head

    def __str__(self):
        return str(self.head) + ' :- ' + ','.join([str(a) for a in self.body]) + '.'
    
    def get_predicates(self):
        return [self.head.predicate] + [a.predicate for a in self.body]

    def copute_mappings(self, rule2):
        variables_1 = []
        variables_1.append(self.head.arguments[0].name)
        var = self.head.arguments[1].name
        if var not in variables_1:
            variables_1.append(var)
        for body_atom in self.body:
            var = body_atom.arguments[0].name
            if var not in variables_1:
                variables_1.append(var)
            var = body_atom.arguments[1].name
            if var not in variables_1:
                variables_1.append(var)
        variables_2 = []
        variables_2.append(rule2.head.arguments[0].name)
        var = rule2.head.arguments[1].name
        if var not in variables_2:
            variables_2.append(var)
        for body_atom in rule2.body:
            var = body_atom.arguments[0].name
            if var not in variables_2:
                variables_2.append(var)
            var = body_atom.arguments[1].name
            if var not in variables_2:
                variables_2.append(var)
        mappings = []
        mappings_head = []
        if self.head.arguments[0].name == self.head.arguments[1].name:
            if rule2.head.arguments[0].name == rule2.head.arguments[1].name:
                mappings_head.append({self.head.arguments[0].name: rule2.head.arguments[0].name})
            elif rule2.head.arguments[0].name != rule2.head.arguments[1].name:
                mappings_head.append({self.head.arguments[0].name: rule2.head.arguments[0].name}, {self.head.arguments[0].name: rule2.head.arguments[1].name})
        elif self.head.arguments[0].name != self.head.arguments[1].name:
            if rule2.head.arguments[0].name != rule2.head.arguments[1].name:
                mappings_head.append({self.head.arguments[0].name: rule2.head.arguments[0].name, self.head.arguments[1].name: rule2.head.arguments[1].name })
            elif rule2.head.arguments[0].name == rule2.head.arguments[1].name:
                mappings_head.append({self.head.arguments[0].name: rule2.head.arguments[0].name}, {self.head.arguments[1].name: rule2.head.arguments[0].name})
        for map in mappings_head:
            mapped_var_1 = []
            mapped_var_2 = []
            for i in map:
                mapped_var_1.append(i)
                mapped_var_2.append(map[i])
            to_map_1= [i for i in variables_1 if i not in mapped_var_1]
            to_map_2= [i for i in variables_2 if i not in mapped_var_2]
            if to_map_1 == [] or to_map_2 == []:
                mappings.append(map)
            else:
                if len(to_map_1)>=len(to_map_2):
                    combo = list(itertools.combinations(to_map_1, len(to_map_2)))
                    for (i, j) in list(itertools.combinations(to_map_1, len(to_map_2))):
                        combo.append((j, i))
                    for element in combo:
                        mapping= map.copy()
                        index = 0
                        for i in element:
                            mapping[i] = to_map_2[index]
                            index += 1
                        mappings.append(mapping)
                else:
                    combo = list(itertools.combinations(to_map_2, len(to_map_1)))
                    for (i, j) in list(itertools.combinations(to_map_2, len(to_map_1))):
                        combo.append((j, i))
                    for element in combo:
                        mapping = map.copy()
                        index = 0
                        for i in element:
                            mapping[to_map_1[index]] = i
                            index += 1
                        mappings.append(mapping)
        return mappings

    def compute_distance(self, rule2):
        '''
        copute distance between 2 rules with same head predicate
        '''
        # return vector with all the metrics
        # TODO: find different mappings, then use the max between the mappings
        # TODO: UPDATE compute_distance ATOMS

        assert self.head.predicate.name == rule2.head.predicate.name, "Error - compute distance between rules with different head predicate!!!!"
        mappings = self.copute_mappings(rule2)
        # TODO: aggiungere i mappings
        mapping_distance = 1
        for mapping in mappings:
            ###################################
            head_dist = self.head.compute_distance(rule2.head, mapping)
            combinations_bodies = []
            if len(self.body) >= len(rule2.body):
                current_distance = len(self.body) - len(rule2.body)
                min_combination_bodies = len(rule2.body)
                long_body = self.body
                short_body = rule2.body
                combo = list(itertools.combinations(long_body, len(short_body)))
                for (i, j) in list(itertools.combinations(long_body, len(short_body))):
                    combo.append((j, i))
                for element in combo:
                    combination_pairs=[]
                    index=0
                    for i in element:
                        combination_pairs.append([i, short_body[index]])
                        index += 1
                    combinations_bodies.append(combination_pairs)
            else:
                current_distance = len(rule2.body) - len(self.body)
                min_combination_bodies = len(self.body)
                long_body = rule2.body
                short_body = self.body
                combo = list(itertools.combinations(long_body, len(short_body)))
                for (i,j) in list(itertools.combinations(long_body, len(short_body))):
                    combo.append((j,i))
                for element in combo:
                    combination_pairs = []
                    index = 0
                    for i in element:
                        combination_pairs.append([short_body[index], i])
                        index += 1
                    combinations_bodies.append(combination_pairs)
            for cc in combinations_bodies:
                # find cost combination
                combination_dist = 0
                for ii in cc:
                    combination_dist+= ii[0].compute_distance(ii[1], mapping)
                if combination_dist < min_combination_bodies:
                    min_combination_bodies = combination_dist
            current_distance += float((head_dist + min_combination_bodies))
            current_distance /= (1+max(len(self.body), len(rule2.body)))
            ###################################
            if current_distance < mapping_distance:
                current_distance = mapping_distance
        return mapping_distance



class Predicate:
    '''
    Parameters
    ----------
    name
    arity
    '''
    def __init__(self, name, arity=0):
        self.name = name
        self.arity = arity

    def __eq__(self, other):
        if isinstance(other, Predicate):
            return self.name == other.name and self.arity == other.arity
        return self is other  #super(Predicate, self).__eq__(other) #TODO

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return 'p'+ str(self.name) if isinstance(self.name, int) else str(self.name)


class Atom(object):
    '''
    Parameters
    ----------
    predicate   :   Predicate
    arguments   :   Term list
    '''
    def __init__(self, predicate, arguments):
        if not len(arguments) == predicate.arity:
            raise Exception("Error: number of arguments "+str(predicate)+" is not "+str(len(arguments))+" but " + str(predicate.arity))
        self.predicate = predicate
        self.arguments = arguments

    def __eq__(self, other):
        if isinstance(other, Atom):
            return self.predicate == other.predicate and self.predicate.arity == sum([1 if other.arguments[i] == a else 0 for i,a in enumerate(self.arguments)])  #self.arguments == other.arguments
        return super(Atom, self).__eq__(other)

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return str(self.predicate) + '(' + ','.join([str(a) for a in self.arguments]) + ')'

    def compute_distance(self, atom2, mapping):
        # Nienhuys-Cheng distance between atoms
        var1_atom2 = atom2.arguments[0].name
        var2_atom2 = atom2.arguments[1].name
        a= mapping.keys()
        if self.arguments[0].name in mapping.keys():
            var1_atom1 = mapping[self.arguments[0].name]
        else:
            var1_atom1 = self.arguments[0].name
        if self.arguments[1].name in mapping.keys():
            var2_atom1 = mapping[self.arguments[1].name]
        else:
            var2_atom1 = self.arguments[1].name
        if self.predicate.name == atom2.predicate.name and var1_atom1 == var1_atom2 and var2_atom1 == var2_atom2 :
            return 0
        elif self.predicate.name != atom2.predicate.name:
            return 1
        else:
            d1 = 1
            if var1_atom1 == var1_atom2 :
                d1 = 0
            d2 = 1
            if var2_atom1 == var2_atom2 :
                d2 = 0
            distance = 0.25 * (d1 + d2)
        return distance




class Term(object):
    # temp needed since only python 3 raises no error when it's missing and addressed
    def __eq__(self, other):
        if self is other:
            return True
        return super(Term, self) == other


class Variable(Term):
    '''
    Parameters
    ----------
    name
    '''
    def __init__(self, name):
        # super().__init__()
        self.name = name

    def __eq__(self, other):
        if isinstance(other, Variable):
            return self.name == other.name
        # return super(Variable, self).__eq__(other)

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return 'X'+ str(self.name) if isinstance(self.name, int) else str(self.name)


class Constant(Term):
    '''
    Parameters
    ----------
    name
    '''
    def __init__(self, name):
        self.name = name

    def __eq__(self, other):
        if isinstance(other, Constant):
            return self.name == other.name
        # return super(Constant, self).__eq__(other)

    def __hash__(self):
        return self.name

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return 'c' + str(self.name) if isinstance(self.name, int) else str(self.name)


#TODO update comments
def forward_inference(facts, rules):
    '''
    Parameters
    ----------
    facts   :   list of lists, where facts[i] is a list af all the facts of predicate p_i
    rules   :   Rule list
    '''
    sys.setrecursionlimit(1000000)
    if rules == []:
        return facts
    updated = True;
    while updated:
        updated = False
        for r in rules:
            # print(r)
            # list of unifiers
            # each unifier is a list of constants
            allus = [{}]
            for a in r.body:
                allustmp = []
                for u in allus:
                    # added these to lines to handle invented predicates
                    if not a.predicate.name in facts.keys():
                        facts[a.predicate.name] = []
                    for f in facts[a.predicate.name]:
                        # print(f)
                        # extend a copy of the current unifier u
                        u1 = unify(f, a, copy.deepcopy(u))
                        if u1 is None:
                            continue
                        # print(dict_tostr(u1))
                        allustmp.append(u1)
                allus = allustmp
            for u in allus:
                f = create_consequence(r.head, u)
                if r.head.predicate.name in facts:
                    if f not in facts[r.head.predicate.name]:
                        updated = True
                        facts[r.head.predicate.name].append(f)
    #                    print(facts)
                else:
                    facts[r.head.predicate.name] = [f]
    return facts


def unify(fact, atom, unifier):
    '''
    Parameters
    ----------
    fact   :   Atom
    atom    :   Atom
    body atom
    unifier    :   Constant list
    constants unified with previous body atoms' terms
    --
    note: method does not check predicates, this is done earlier
    Example:
    fact: p4('a','e','c')
    atom: p4('a',x7,x2) (3rd from above example rule)
    unifier: ['a','b','c','a','d','c'] (list has length 6 since
    other atoms have to be processed before and are ternary;
    double occurrence of 'a' and 'c' comes from 2nd atom)
    -
    matches since fact starts with 'a', 7 indicates arbitrary match, fact's 'c'
    corresponds to element at index 2 in list
    we extend unifier to ['a','b','c','a','d','c','a','e','c']
    '''
    # print('got '+dict_tostr(unifier))
    # print(atom)
    for i in range(0, atom.predicate.arity):
        # print(str(atom.arguments[i].name))
        if isinstance(atom.arguments[i], Constant):
            if fact.arguments[i].name != atom.arguments[i].name:
                return None
            continue
        elif atom.arguments[i].name in unifier:
            # print('in ')
            if fact.arguments[i].name != unifier[atom.arguments[i].name]:
                # print('bad '+str(atom.arguments[i]) + " "+str(fact.arguments[i]) + " "+str(unifier[atom.arguments[i].name]))
                return None
        #else var can match any constant
        # print('... ')
        # print('a '+str(atom.arguments[i]) + " "+str(fact.arguments[i]) )#+ " "+str(unifier[atom.arguments[i].name]))
        unifier[atom.arguments[i].name]=fact.arguments[i].name
        # print('now '+dict_tostr(unifier))
    return unifier


def create_consequence(head, unifier):
    '''
    Parameters
    ----------
    head   :   Atom
    unifier  :   Constant list
    constants unified with body atoms' terms
    --
    Example:
    head: p6(x0,x7)
    unifier: ['a','b','c','a','d','c','a','e','c']
    -
    return p6('a','e')

    '''
    # print('u '+dict_tostr(unifier))
    args = [head.arguments[i] if isinstance(head.arguments[i], Constant)
            else Constant(unifier[head.arguments[i].name])
            for i in range(0, head.predicate.arity)]
    return Atom(head.predicate, args)


def dict_tostr(dic):
    return ",".join([str(k)+":"+(strlist(v)+'\n' if isinstance(v, list) else str(v)) for k, v in dic.items() if v])#TODO change for python 3


def test_consistency(facts, rules):
    # TODO: not sure we need this
    pass


def strlist(objects):
    return str([str(o) for o in objects])


def strlist_names(l):
    return str([str(c.name) for c in l])


def atoms_tostr(facts):
    return [", ".join([str(c) for c in fs]) for fs in facts.values() if fs]


if __name__ == '__main__':
    print("Testing LogicClasses")
# #   test unify
#     a = Constant(0)
#     b = Constant(1)
#     c = Constant(2)
#     d = Constant(3)
#     e = Constant(4)
#     x0 = Variable(5)
#     x1 = Variable(1)
#     x2 = Variable(2)
#     x3 = Variable(3)
#     p0 = Predicate(0,3)
#     p1 = Predicate(1,3)
#     p2 = Predicate(2,2)
#     f1 = Atom(p0,[a,b,c])
#     f2 = Atom(p0,[a,b,a])
#     f2copy = Atom(p0,[a,b,Constant(0)])
#     a1 = Atom(p0,[x0,x1,x2])
#     a2 = Atom(p0,[x0,x1,x0])
#     a3 = Atom(p0,[x3,x1,x2])
#     a4 = Atom(p2,[x0,x0])
#
#     a1c = Atom(p0,[x0,b,x2])
#     a2c = Atom(p0,[x0,x1,a])
#     a3c = Atom(p0,[x3,b,x2])
#
# #    u = unify(f1, a1, [])
# #    print(u == [a,b,c])
# #
# #    u = unify(f2, a1, [])
# #    print(u == [a,b,a])
# #
# #    u = unify(f2, f2copy, [])
# #    print(u == [])
# #
# #    u = unify(f1, a1c, [])
# #    print(u == [a,b,c])
# #
# #    u = [a,b,c]
# #    u1 = unify(f2, a3, copy.deepcopy(u))
# #    print(u1 == [])
# #
# #    u = [a,b,c,a,b,c]
# #    u1 = unify(f1, a3, copy.deepcopy(u))
# #    print(u1 == [a,b,c,a,b,c,a,b,c])
#
#
# # #   test inference TODO
#

#    u = [a,b,c,a,b,c]
#    u1 = unify(f1, a3, copy.deepcopy(u))
#    print(u1 == [a,b,c,a,b,c,a,b,c])
    '''
# #   test inference TODO

    p0 = Predicate(0,1)
    fact=Atom(p0,[Constant(1)] ) #p0(c0,c1)
#    head=Atom(Predicate(1,1),[Variable(0)] )
#    body=[Atom(Predicate(0,2),[Variable(0),Constant(1)])]
#    body=[Atom(Predicate(0,2),[Variable(0),Constant(1)])]
#    rule=Rule(body,head)  #p1(X0):- p0(X0,X1).
    a=forward_inference({0:[fact],1:[]},[\
            Rule([Atom(p0,[Constant(1)])],Atom(p0,[Constant(0)]))])
    print(atoms_tostr(a))
    '''
    '''
# #   test inference -- Cristina
    p0 = Predicate(0,1)
    fact = Atom(p0,[Constant(1)] ) #p0(c1)
    head = Atom(Predicate(1,1),[Variable(0)] )
    body = [Atom(Predicate(0,1),[Variable(0)]),Atom(Predicate(0,1),[Constant(3)]),Atom(Predicate(0,1),[Variable(0)])]
    rule = Rule(body,head)  #p1(X0):- p0(X0).
    dict = {0:[fact],1:[]}
    a = forward_inference(dict,[rule])
    print(atoms_tostr(a))
    '''
    '''
# test empty body rules
    p0 = Predicate(0,1)
    fact=Atom(p0,[Constant(1)] ) #p0(c1)
    head=Atom(Predicate(1,1),[Variable(0)] )
    body=[Atom(Predicate(0,1),[Variable(0)])]
    rule=Rule(None,head)  #p1(X0):- p0(X0).
    dict={0:[fact],1:[]}
    a = forward_inference(dict,[rule])
    print(atoms_tostr(a))
    '''

# #   test inference -- Cristina
    even = Predicate('even', 1)
    succ = Predicate('succ', 2)
    fact1 = Atom(succ, [Constant('0'), Constant('1')])  # succ(0,1)
    fact2 = Atom(succ, [Constant('1'), Constant('2')] )  # succ(1,2)
    fact3 = Atom(even, [Constant('0')])  # even(0)
    head = Atom(even, [Variable('C')])
    body = [Atom(succ, [Variable('A'), Variable('B')]), Atom(even, [Variable('A')]), Atom(succ, [Variable('B'), Variable('C')])]
    rule = Rule(body, head)  # even(C) :- succ(A,B), even(A), succ(B,C).
    dict = {'even': [fact3], 'succ':[fact1, fact2]}
    a = forward_inference(dict, [rule])
    print(atoms_tostr(a))


import copy
import os
import src.logic as log


def write_metadata_file_mitibm(rules, facts, max_steps, path, name):
    with open(path + name, "w") as f:
        f.write("task_type: evans_ilp\n")
        f.write("background_predicates:\n")
        for p in facts:
            print(p)
            if facts[p]:
                f.write(" - " + str(p) + "\n")
        # f.write("auxiliary_predicates:\n")
        # f.write(" names:\n")
        #
        # f.write(" arities:\n")

        f.write("target_predicates:\n")  # TODO check this if remember correctly
        for r in rules:  # TODO currently predicates can occur multiple times, is this ok?
            f.write(" - " + str(r.head.predicate) + "\n")
        f.write("inference_steps: " + str(max_steps) + "\n") #TODO is it ok to have an ub here?
        f.write("templates:\n")
        for r in rules:  # TODO predicate printing, constants by same vars? - but make Y
            r1 = copy.deepcopy(r)
            r1.head.predicate.name = "F"
            r1.head.arguments = [t if isinstance(t, log.Variable) else log.Variable("Y" + str(t.name)) for t in
                                 r1.head.arguments]
            for a in r1.body:
                a.predicate.name = "F"
                a.arguments = [t if isinstance(t, log.Variable) else log.Variable("Y" + str(t.name)) for t in
                               a.arguments]
            f.write(" - " + str(r1)[0:len(str(r1)) - 1] + "\n")



def write_ntp_rule_template(rules, dstpath):
    terms = ['A','B','C','D','E','F'] #'G','H'
    with open(dstpath , "w") as f:
        for r in rules:  # TODO predicate printing, constants by same vars? - but make Y
            dict = {}
            i = 0
            r1 = copy.deepcopy(r)
            r1.head.predicate.name = "#"+r1.head.predicate.name.replace('p','')
            for t in r1.head.arguments:
                if t.name not in dict:
                    dict[t.name] = terms[i]
                    i += 1
                t.name = dict[t.name]

            for a in r1.body:
                a.predicate.name = "#"+a.predicate.name.replace('p','')
                for t in a.arguments:
                    if t.name not in dict:
                        dict[t.name] = terms[i]
                        i += 1
                    t.name = dict[t.name]
            f.write("20 " + str(r1) + "\n")#[0:len(str(r1)) - 1]

    # with open(path + name +".nlt", "w") as f:
    #     for r in rules:
    #         r1 = copy.deepcopy(r)
    #         i = 2
    #         v = {r1.head.predicate.name:1}
    #         r1.head.predicate = log.Predicate("#" + str(v[r1.head.predicate.name]))
    #         for t in r1.head.arguments:
    #             if isinstance(t, log.Variable):
    #
    #         for a in r1.body:
    #             if a.predicate.name not in v:
    #                 v[a.predicate.name] = i
    #                 i += 1
    #             a.predicate = log.Predicate("#" + str(v[a.predicate.name]))
    #             r1.head.arguments = [t if isinstance(t, log.Variable) else log.Variable("Y" + str(t.name)) for t in
    #                              r1.head.arguments]
    #         for a in r1.body:
    #             a.predicate.name = "F"
    #             a.arguments = [t if isinstance(t, log.Variable) else log.Variable("Y" + str(t.name)) for t in
    #                            a.arguments]
    #         f.write(" - " + str(r1)[0:len(str(r1)) - 1] + "\n")










if __name__ == '__main__':
    path = 'datasets/'
    dataset = 'CHAIN-XS/'
    f_rules = 'rules'
    f_facts = 'train'

    path = path + dataset

    facts, rules, predicates, constants = Evaluator.parseFiles_general(path+f_facts,path+f_rules)
    print(rules)

    path = path + 'metadata/'
    # if os.path.exists(path):
    #     shutil.rmtree(path, ignore_errors=True)
    os.mkdir(path, )

    max_steps = len(rules)
    write_metadata_file_mitibm(rules, facts, max_steps, path, 'mitibm')
import os
if __name__ == '__main__':
    # dataset_dir = sys.argv[1]
    #
    # report_dataset_stats(dataset_dir+'/train.tsv')
    # report_dataset_stats(dataset_dir+'/dev.tsv')
    # report_dataset_stats(dataset_dir+'/test.tsv')
    data  = "../../datasets/exp1/"

    for d in os.listdir(data):
        if d.startswith("."): continue


        entities = set()

        ns = ["/train-cw.txt","/train.txt"]
        for n in ns:
            with open(data + d + n, "r") as facts:
                for f in facts:
                    c = f.index(",")
                    e = f[f.index("(")+1:c]
                    entities.add(e)
                    e = f[c+1:f.index(")")]
                    entities.add(e)

        entities_old = []
        with open(data + d + "/entities.txt", "r") as es:
            for e in es:
                entities_old.append(e.strip())
                if e.strip() not in entities: print(e.strip())
        for e in entities:
            if e not in entities_old:
                print(e)

        if len(set(entities_old)) < len(entities):
            print(d,len(set(entities_old)),len(entities))
        # else: print(d,"*"*50)

        with open(data + d + "/entities.txt", "w") as f:
            f.write("\n".join(list(entities)))

        # break


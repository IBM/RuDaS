import os


DATASETS_EXP_2 = ['CHAIN-XS-2', 'CHAIN-XS-2-0', 'CHAIN-XS-2-1', 'CHAIN-XS-2-2', 'CHAIN-XS-2-3', 'CHAIN-XS-2-4', 'CHAIN-XS-2-5', 'CHAIN-XS-2-6', 'CHAIN-XS-2-7', 'CHAIN-XS-2-8', 'CHAIN-XS-3', 'CHAIN-XS-3-0', 'CHAIN-XS-3-1', 'CHAIN-XS-3-2', 'CHAIN-XS-3-3', 'CHAIN-XS-3-4', 'CHAIN-XS-3-5', 'CHAIN-XS-3-6', 'CHAIN-XS-3-7', 'CHAIN-XS-3-8', 'CHAIN-S-2', 'CHAIN-S-2-0', 'CHAIN-S-2-1', 'CHAIN-S-2-2', 'CHAIN-S-2-3', 'CHAIN-S-2-4', 'CHAIN-S-2-5', 'CHAIN-S-2-6', 'CHAIN-S-2-7', 'CHAIN-S-2-8', 'CHAIN-S-3', 'CHAIN-S-3-0', 'CHAIN-S-3-1', 'CHAIN-S-3-2', 'CHAIN-S-3-3', 'CHAIN-S-3-4', 'CHAIN-S-3-5', 'CHAIN-S-3-6', 'CHAIN-S-3-7', 'CHAIN-S-3-8', 'RDG-XS-2', 'RDG-XS-2-0', 'RDG-XS-2-1', 'RDG-XS-2-2', 'RDG-XS-2-3', 'RDG-XS-2-4', 'RDG-XS-2-5', 'RDG-XS-2-6', 'RDG-XS-2-7', 'RDG-XS-2-8', 'RDG-XS-3', 'RDG-XS-3-0', 'RDG-XS-3-1', 'RDG-XS-3-2', 'RDG-XS-3-3', 'RDG-XS-3-4', 'RDG-XS-3-5', 'RDG-XS-3-6', 'RDG-XS-3-7', 'RDG-XS-3-8', 'RDG-S-2', 'RDG-S-2-0', 'RDG-S-2-1', 'RDG-S-2-2', 'RDG-S-2-3', 'RDG-S-2-4', 'RDG-S-2-5', 'RDG-S-2-6', 'RDG-S-2-7', 'RDG-S-2-8', 'RDG-S-3', 'RDG-S-3-0', 'RDG-S-3-1', 'RDG-S-3-2', 'RDG-S-3-3', 'RDG-S-3-4', 'RDG-S-3-5', 'RDG-S-3-6', 'RDG-S-3-7', 'RDG-S-3-8', 'DRDG-XS-2', 'DRDG-XS-2-0', 'DRDG-XS-2-1', 'DRDG-XS-2-2', 'DRDG-XS-2-3', 'DRDG-XS-2-4', 'DRDG-XS-2-5', 'DRDG-XS-2-6', 'DRDG-XS-2-7', 'DRDG-XS-2-8', 'DRDG-XS-3', 'DRDG-XS-3-0', 'DRDG-XS-3-1', 'DRDG-XS-3-2', 'DRDG-XS-3-3', 'DRDG-XS-3-4', 'DRDG-XS-3-5', 'DRDG-XS-3-6', 'DRDG-XS-3-7', 'DRDG-XS-3-8', 'DRDG-S-2', 'DRDG-S-2-0', 'DRDG-S-2-1', 'DRDG-S-2-2', 'DRDG-S-2-3', 'DRDG-S-2-4', 'DRDG-S-2-5', 'DRDG-S-2-6', 'DRDG-S-2-7', 'DRDG-S-2-8', 'DRDG-S-3', 'DRDG-S-3-0', 'DRDG-S-3-1', 'DRDG-S-3-2', 'DRDG-S-3-3', 'DRDG-S-3-4', 'DRDG-S-3-5', 'DRDG-S-3-6', 'DRDG-S-3-7', 'DRDG-S-3-8']
DATASETS_EXP_1 = ['RDG-XS-2', 'RDG-XS-2-0', 'RDG-XS-2-1', 'RDG-XS-2-10', 'RDG-XS-2-2', 'RDG-XS-2-3', 'RDG-XS-2-4', 'RDG-XS-2-5', 'RDG-XS-2-6', 'RDG-XS-2-7', 'RDG-XS-2-8', 'RDG-XS-2-9', 'RDG-XS-3', 'RDG-XS-3-0', 'RDG-XS-3-1', 'RDG-XS-3-10', 'RDG-XS-3-2', 'RDG-XS-3-3', 'RDG-XS-3-4', 'RDG-XS-3-5', 'RDG-XS-3-6', 'RDG-XS-3-7', 'RDG-XS-3-8', 'RDG-XS-3-9', 'RDG-S-2', 'RDG-S-2-0', 'RDG-S-2-1', 'RDG-S-2-10', 'RDG-S-2-2', 'RDG-S-2-3', 'RDG-S-2-4', 'RDG-S-2-5', 'RDG-S-2-6', 'RDG-S-2-7', 'RDG-S-2-8', 'RDG-S-2-9', 'RDG-S-3', 'RDG-S-3-0', 'RDG-S-3-1', 'RDG-S-3-10', 'RDG-S-3-2', 'RDG-S-3-3', 'RDG-S-3-4', 'RDG-S-3-5', 'RDG-S-3-6', 'RDG-S-3-7', 'RDG-S-3-8', 'RDG-S-3-9', 'DRDG-XS-2', 'DRDG-XS-2-0', 'DRDG-XS-2-1', 'DRDG-XS-2-10', 'DRDG-XS-2-2', 'DRDG-XS-2-3', 'DRDG-XS-2-4', 'DRDG-XS-2-5', 'DRDG-XS-2-6', 'DRDG-XS-2-7', 'DRDG-XS-2-8', 'DRDG-XS-2-9', 'DRDG-XS-3', 'DRDG-XS-3-0', 'DRDG-XS-3-1', 'DRDG-XS-3-10', 'DRDG-XS-3-2', 'DRDG-XS-3-3', 'DRDG-XS-3-4', 'DRDG-XS-3-5', 'DRDG-XS-3-6', 'DRDG-XS-3-7', 'DRDG-XS-3-8', 'DRDG-XS-3-9', 'DRDG-S-2', 'DRDG-S-2-0', 'DRDG-S-2-1', 'DRDG-S-2-10', 'DRDG-S-2-2', 'DRDG-S-2-3', 'DRDG-S-2-4', 'DRDG-S-2-5', 'DRDG-S-2-6', 'DRDG-S-2-7', 'DRDG-S-2-8', 'DRDG-S-2-9', 'DRDG-S-3', 'DRDG-S-3-0', 'DRDG-S-3-1', 'DRDG-S-3-10', 'DRDG-S-3-2', 'DRDG-S-3-3', 'DRDG-S-3-4', 'DRDG-S-3-5', 'DRDG-S-3-6', 'DRDG-S-3-7', 'DRDG-S-3-8', 'DRDG-S-3-9']



def print_measures(file_name,FOIL,NTP,NEURAL_LP,AMIE):
	file = open(file_name, "w")
	file.write("\n FOIL")
	for i in FOIL:
		file.write(i+"\n")
	file.write("\n NTP")
	for i in NTP:
		file.write(i+"\n")
	file.write("\n NEURAL_LP")
	for i in NEURAL_LP:
		file.write(i+"\n")
	file.write("\n AMIE")
	for i in AMIE:
		file.write(i+"\n")


def read_measures(file_name):
	no_eval=False
	measures=[" "," "," "," "," "," "]
	Herbrant_accuracy=None
	Hscore=None
	Accuracy=None
	Precision=None
	Recall=None
	F1score=None
	try:
		file = open(file_name, "r")
	except:
		no_eval=True
	if no_eval:
		return measures
	else:
		line = fp.readline()
		while line:
			if "Herbrant accuracy" in line:
				Herbrant_accuracy=line.split("\t")[1]
			if "Hscore" in line:
				Hscore=line.split("\t")[1]
			if "Accuracy" in line:
				Accuracy=line.split("\t")[1]
			if "Precision" in line:
				Precision=line.split("\t")[1]
			if "Recall" in line:
				Recall=line.split("\t")[1]
			if "F1score" in line:
				F1score=line.split("\t")[1]
		measures=[Herbrant_accuracy,Hscore,Accuracy,Precision,Recall,F1score]
	file.close()
	return measures


def read_exp_2():
	path="OUTPUT2/exp2/"
	datasets = DATASETS_EXP_2
	# FOIL
	FOIL_Herbrant_accuracy=[]
	FOIL_Hscore=[]
	FOIL_Accuracy=[]
	FOIL_Precision=[]
	FOIL_Recall=[]
	FOIL_F1score=[]
	# NTP
	NTP_Herbrant_accuracy=[]
	NTP_Hscore=[]
	NTP_Accuracy=[]
	NTP_Precision=[]
	NTP_Recall=[]
	NTP_F1score=[]
	# NEURAL_LP
	NEURAL_LP_Herbrant_accuracy=[]
	NEURAL_LP_Hscore=[]
	NEURAL_LP_Accuracy=[]
	NEURAL_LP_Precision=[]
	NEURAL_LP_Recall=[]
	NEURAL_LP_F1score=[]
	# AMIE
	AMIE_Herbrant_accuracy=[]
	AMIE_Hscore=[]
	AMIE_Accuracy=[]
	AMIE_Precision=[]
	AMIE_Recall=[]
	AMIE_F1score=[]
	# FOIL
	for dataset in datasets:
		mes=read_measures(path+"FOIL/"+dataset+"INCOMPLETE_NOISE2/evaluation.txt")
		FOIL_Herbrant_accuracy.append(mes[0])
		FOIL_Hscore.append(mes[1])
		FOIL_Accuracy.append(mes[2])
		FOIL_Precision.append(mes[3])
		FOIL_Recall.append(mes[4])
		FOIL_F1score.append(mes[5])
	# NTP
	for dataset in datasets:
		mes=read_measures(path+"ntp/"+dataset+"INCOMPLETE_NOISE2/evaluation.txt")
		# NTP
		NTP_Herbrant_accuracy.append(mes[0])
		NTP_Hscore.append(mes[1])
		NTP_Accuracy.append(mes[2])
		NTP_Precision.append(mes[3])
		NTP_Recall.append(mes[4])
		NTP_F1score.append(mes[5])
	# NEURAL_LP
	for dataset in datasets:
		mes=read_measures(path+"Neural-LP/"+dataset+"INCOMPLETE_NOISE2/evaluation.txt")
		NEURAL_LP_Herbrant_accuracy.append(mes[0])
		NEURAL_LP_Hscore.append(mes[1])
		NEURAL_LP_Accuracy.append(mes[2])
		NEURAL_LP_Precision.append(mes[3])
		NEURAL_LP_Recall.append(mes[4])
		NEURAL_LP_F1score.append(mes[5])
	# AMIE
	for dataset in datasets:
		mes=read_measures(path+"amiep/"+dataset+"INCOMPLETE_NOISE2/evaluation.txt")
		AMIE_Herbrant_accuracy.append(mes[0])
		AMIE_Hscore.append(mes[1])
		AMIE_Accuracy.append(mes[2])
		AMIE_Precision.append(mes[3])
		AMIE_Recall.append(mes[4])
		AMIE_F1score.append(mes[5])


	print_measures("2_Herbrant_accuracy.txt",FOIL_Herbrant_accuracy,NTP_Herbrant_accuracy,NEURAL_LP_Herbrant_accuracy,AMIE_Herbrant_accuracy)
	print_measures("2_Hscore.txt",FOIL_Hscore,NTP_Hscore,NEURAL_LP_Hscore,AMIE_Hscore)
	print_measures("2_Accuracy.txt",FOIL_Accuracy,NTP_Accuracy,NEURAL_LP_Accuracy,AMIE_Accuracy)
	print_measures("2_Precision.txt",FOIL_Precision,NTP_Precision,NEURAL_LP_Precision,AMIE_Precision)
	print_measures("2_Recall.txt",FOIL_Recall,NTP_Recall,NEURAL_LP_Recall,AMIE_Recall)
	print_measures("2_F1score.txt",FOIL_F1score,NTP_F1score,NEURAL_LP_F1score,AMIE_F1score)


def read_exp_1():
	path="OUTPUT1/exp1/"
	datasets = DATASETS_EXP_1


read_exp_2()

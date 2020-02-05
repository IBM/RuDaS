# Experiments

Installation requirements: git, anaconda, wget
1. Run **experiments/setup_systems_scripts/setup_systems.sh** to download and set up the systems.
2. Run **experiments/src/experiments_generation.py** to set up the experiments.
3. Run **experiments/run_scripts/run_exp1_complete.sh**, **experiments/run_scripts/run_exp1_incomplete.sh** and **experiments/run_scripts/run_exp1_incomplete_noise.sh** to run experiment 1.
4. Run **experiments/run_scripts/run_exp2_2.sh** to run experiment 2.
4. For evaluation, run **experiments/src/experiments_evaluation.py** to evaluate both experiments.

The following systems are evaluated:

## FOIL

* *Version:* 6.4
* *Source Code:* <http://www.rulequest.com/Personal/>
 
## Amie+
* *Paper:* 
Fast Rule Mining in Ontological Knowledge Bases with AMIE+.
Luis Galárraga, Christina Teflioudi, Fabian Suchanek, Katja Hose.
VLDB Journal 2015. 
<https://suchanek.name/work/publications/vldbj2015.pdf>
* *Source Code:* <https://www.mpi-inf.mpg.de/departments/databases-and-information-systems/research/yago-naga/amie/>

## Neural-LP

* *Paper:* 
Differentiable Learning of Logical Rules for Knowledge Base Reasoning. 
Fan Yang, Zhilin Yang, William W. Cohen. 
NIPS 2017.
<https://arxiv.org/abs/1702.08367>
* *Source Code:*
<https://github.com/fanyangxyz/Neural-LP>

## ntp

* *Paper:*
End-to-end Differentiable Proving. 
Tim Rocktaeschel and Sebastian Riedel. 
NIPS 2017. 
<http://papers.nips.cc/paper/6969-end-to-end-differentiable-proving>
* *Source Code:*
<https://github.com/uclmr/ntp>

<!--## neural-theorem-proving (not sure anymore if we should test that)

* *Paper:*
Logical Rule Induction and Theory Learning Using Neural Theorem Proving.
Andres Campero, Aldo Pareja, Tim Klinger, Josh Tenenbaum, and Sebastian Riedel.
<https://arxiv.org/abs/1809.02193>
* *Source Code:*
<https://git.ng.bluemix.net/MIT-IBM-Watson-AI-Lab/AI-Algorithms/inducing-probabilistic-programs/neural-theorem-proving>-->



# Experiments results

## Overall Results in Terms of Different Metrics
Impact of different metrics, each one averaged on 120 datasets with uniformly distributed categories CHAIN, RDG, DRDG, sizes in {XS,S}, and graph depths in {2,3}; n_OW=0.3, nNoise-=0.2, nNoise+=0.1.

|| FOIL | AMIE+ | Neural-LP | NTP |
| --- | --- | ---  | --- | --- | 
| **H-accuracy** | 0.9873 | 0.8498 | 0.9850 |0.9221 |
|    **Accuracy** | 0.9872  | 0.8494 | 0.9849   | 0.9219|
 |   **F1-score** |  0.2151 | 0.3031 |  0.1621 | 0.1125 |
  |  **H-score** | 0.1520 | 0.2321 | 0.1025 | 0.0728 |
 |   **Precision** | 0.5963 | 0.2982 | 0.1687 | 0.1021 | %($s_c$) 
 |   **Recall** | 0.2264  | 0.7311  | 0.2433 | 0.3921 |
 |   **R-score** | 0.2728  | 0.3350  | 0.1906 | 0.1811 |

## Impact of Missing Consequences and Noise
Effect of missing consequences and noise on 144 datasets. Each H-score value is averaged on 48 datasets, with uniformly distributed categories in {RDG, DRDG}, sizes in {XS,S}, and graph depths in {2,3}
The  noise  parameters  are  definesas follows (the  set  memberships  are  intended  to  mean  “uniformly  distributed over"):

|                        |  n_OW       |  nNoise+ |  nNoise-   | 
| ---                    |  ---        |  ---     | ---        | 
| **Complete**           |    0        |    0     |     0      | 
| **Incomplete**         |{0.2,0.3,0.4}| 0        |  0         |
| **Incomplete + Noise** |{0.2,0.3,0.4}| {0.2,0.3}| {0.15,0.3} | 

Moreover, in order to give an impression  of  some  of  the  datasets  considered  in  existing  evaluations,  we  included  one  manually  created  dataset,  EVEN,inspired  by  the  corresponding  dataset  used  in  (Evans  and Grefenstette 2018), which contains complete information:

|| EVEN |  Complete | Incomplete | Incomplete + Noise | 
| --- | --- | ---  | --- | --- | 
| **FOIL**     | 1.0  | 0.4053  | 0.1919  | 0.0849  |
| **AMIE+**    | -    |  0.2021 |  0.2098 |  0.2075 |
| **Neural-LP**|  -   | 0.0633  | 0.0692  | 0.0649  |
| **NTP**      | 1.0  | 0.0482  | 0.0617  | 0.0574  |

## Impact of Dependencies Between Rules
Impact of dataset category. H-score averaged on 40datasets with uniformly distributed categories CHAIN, RDG, DRDG, sizes in {XS,S}, and graph depths in {2,3}; n_OW=0.3, nNoise-=0.2, nNoise+=0.1.

||CHAIN |  RDG | DRDG  |
| --- | --- | ---  | --- |
| **FOIL** | 0.2024 | 0.0877 | 0.1633     |
| **AMIE+** |  0.3395 | 0.2275 | 0.1293 |
| **Neural-LP**  |  0.1291 | 0.1050 | 0.0734 |
| **NTP**  | 0.1239 | 0.0538 | 0.0368   |

## Scalability: Impact of Dataset Size
Impact of dataset size and rule graph depth. H-scoreaveraged on 30 datasets with uniformly distributed categories CHAIN, RDG, DRDG, sizes in {XS,S}, and graph depths in {2,3}; n_OW=0.3, nNoise-=0.2, nNoise+=0.1.

|| XS-2 |  XS-3 | S-2 | S-3  |
| --- | --- | ---  | --- | --- | 
| **FOIL**  | 0.2815 | 0.2074 | 0.0356 | 0.0934 |
| **AMIE+**  | 0.1449 | 0.1319 | 0.4392 | 0.2124 |
| **Neural-LP**   | 0.1155 | 0.0673 | 0.1281 | 0.0992 |
| **NTP** | 0.1512 | 0.0432 | 0.0652 | 0.0374|

Same results aggregated by sizes in {XS,S}:

|| XS | S   |
| --- | --- | --- | 
| **FOIL**  | 0.2444 | 0.0645 |
| **AMIE+**  | 0.1384 | 0.3258 |
| **Neural-LP**   | 0.0914 | 0.1137 |
| **NTP** | 0.0972 | 0.0513 |

Same results aggregated by graph depths in {2,3}:

|| 2 | 3   |
| --- | --- | --- | 
| **FOIL**  | 0.1585 | 0.1504 |
| **AMIE+**  | 0.2921 | 0.1722 |
| **Neural-LP**   | 0.1218 | 0.0833 |
| **NTP** | 0.1082 | 0.0403 |

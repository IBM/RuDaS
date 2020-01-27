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
| **H-accuracy** |0.9872 | 0.8708 | 0.9852 |0.9304 |
|    **Accuracy** | 0.9872  | 0.8719 | 0.9850   | 0.9302|
 |   **F1-score** |  0.2136 | 0.3164 |  0.1620 | 0.1192 |
  |  **H-score** | 0.1523 | 0.2429 | 0.1027 | 0.0772 |
 |   **Precision** | 0.5810 | 0.3125 | 0.1693 | 0.1049 | %($s_c$) 
 |   **Recall** | 0.2273  | 0.7178  | 0.2421 | 0.3960 |


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
| **FOIL** | 1.0 | 0.3951 | 0.2102 | 0.0940 |
| **AMIE+** | - |  0.2219 |  0.2646 |  0.2634 |
|  **Neural-LP**|  - | 0.0659 | 0.0750 | 0.0701 |
|   **NTP**  | 1.0 | 0.0601 | 0.0833 | 0.0718|

## Impact of Dependencies Between Rules
Impact of dataset category. H-score averaged on 40datasets with uniformly distributed categories CHAIN, RDG, DRDG, sizes in {XS,S}, and graph depths in {2,3}; n_OW=0.3, nNoise-=0.2, nNoise+=0.1.

||CHAIN |  RDG | DRDG  |
| --- | --- | ---  | --- |
| **FOIL** | 0.2024 | 0.0873 | 0.1648     |
| **AMIE+** |  0.3395 | 0.2323 | 0.1443 |
| **Neural-LP**  |  0.1291 | 0.1059 | 0.0718 |
| **NTP**  | 0.1239 | 0.0551 | 0.0427   |

## Scalability: Impact of Dataset Size
Impact of dataset size and rule graph depth. H-scoreaveraged on 30 datasets with uniformly distributed categories CHAIN, RDG, DRDG, sizes in {XS,S}, and graph depths in {2,3}; n_OW=0.3, nNoise-=0.2, nNoise+=0.1.

|| XS-2 |  XS-3 | S-2 | S-3  |
| --- | --- | ---  | --- | --- | 
| **FOIL**  | 0.2815 | 0.2119 | 0.0346 | 0.0934 |
| **AMIE+**  | 0.1449 | 0.1581 | 0.4392 | 0.2124 |
| **Neural-LP**   | 0.1155 | 0.0643 | 0.1281 | 0.0992 |
| **NTP** | 0.1512 | 0.0605 | 0.0562 | 0.0471|

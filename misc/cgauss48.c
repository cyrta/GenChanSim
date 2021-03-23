/* Generate a pair of COMPLEX Gaussian number.  Adapted from Marc Sandler's 
 * Lewis & and Payne gauss r.n.g.
 * The output is zero mean with a standard deviation of 1.0
 * This function is self-contained.
 * The functions 'SaveSeed' and 'GetSeed' may be used to continue in the 
 * sequence.
 * This function should be identical to 'gauss48.c', except that is is looped
 * twice as many times and the numbers are divides the sqrt(2) (because they're
 * complex)
 *
 * Written by Marc Sandler (originally title lpgauss.c),
 * modified by Rich Coutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: cgauss48.c,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: cgauss48.c,v $
 * Changed a (float) cast on index1 to a (double) casting. 6-12-97 CMK
 *
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

 */

#include <stdio.h>
#include <math.h>

#define SIGNMASK 0x80000000	/* to mask off sign bit */
#define CLEAR_10MSB_MASK 0x003FFFFF
#define CLEAR_16MSB_MASK 0x0000FFFF
#define CLEAR_22MSB_MASK 0x000003FF
#define CLEAR_28MSB_MASK 0x0000000F
#define KEEP_9LSB_MASK 0x000001FF
#define	NGAUSS		2048

static char rcsid[] = "$Id: cgauss48.c,v 2.2 1994/07/25 18:30:41 jjb Exp $";

/* forward declaration (this isn't in any /usr/include .h files) */
extern long mrand48();

void cgauss48(float data[], int nsamples) 
{
const double SQRT2 = 1.41421356237;
static float gausstbl[NGAUSS] = {	/* gaussian table */
0.00000000, 0.0024478817, 0.0048957784, 0.0073437043, 0.0097916741, 0.012239702,
0.014687805, 0.017135995, 0.019584287, 0.022032697, 0.024481239, 0.026929928,
0.029378777, 0.031827804, 0.034277022, 0.036726445, 0.039176088, 0.041625965,
0.044076096, 0.046526488, 0.048977163, 0.051428128, 0.053879403, 0.056331001,
0.058782939, 0.061235230, 0.063687891, 0.066140935, 0.068594374, 0.071048230,
0.073502511, 0.075957239, 0.078412421, 0.080868073, 0.083324216, 0.085780859,
0.088238023, 0.090695724, 0.093153968, 0.095612772, 0.098072156, 0.10053214,
0.10299273, 0.10545394, 0.10791579, 0.11037829, 0.11284146, 0.11530532,
0.11776988, 0.12023515, 0.12270116, 0.12516791, 0.12763542, 0.13010372,
0.13257280, 0.13504270, 0.13751341, 0.13998497, 0.14245738, 0.14493066,
0.14740483, 0.14987990, 0.15235589, 0.15483281, 0.15731069, 0.15978953,
0.16226935, 0.16475017, 0.16723201, 0.16971488, 0.17219879, 0.17468376,
0.17716983, 0.17965698, 0.18214524, 0.18463463, 0.18712516, 0.18961686,
0.19210973, 0.19460380, 0.19709909, 0.19959560, 0.20209335, 0.20459236,
0.20709266, 0.20959425, 0.21209714, 0.21460137, 0.21710695, 0.21961389,
0.22212221, 0.22463194, 0.22714306, 0.22965564, 0.23216966, 0.23468514,
0.23720211, 0.23972058, 0.24224058, 0.24476212, 0.24728522, 0.24980989,
0.25233614, 0.25486401, 0.25739354, 0.25992468, 0.26245749, 0.26499200,
0.26752821, 0.27006614, 0.27260581, 0.27514723, 0.27769044, 0.28023544,
0.28278226, 0.28533092, 0.28788143, 0.29043382, 0.29298809, 0.29554430,
0.29810241, 0.30066249, 0.30322453, 0.30578858, 0.30835462, 0.31092271,
0.31349286, 0.31606507, 0.31863937, 0.32121578, 0.32379434, 0.32637504,
0.32895792, 0.33154300, 0.33413029, 0.33671981, 0.33931160, 0.34190568,
0.34450206, 0.34710076, 0.34970179, 0.35230520, 0.35491103, 0.35751924,
0.36012989, 0.36274299, 0.36535859, 0.36797667, 0.37059730, 0.37322044,
0.37584618, 0.37847450, 0.38110545, 0.38373902, 0.38637528, 0.38901421,
0.39165586, 0.39430025, 0.39694741, 0.39959732, 0.40225008, 0.40490565,
0.40756407, 0.41022539, 0.41288960, 0.41555676, 0.41822687, 0.42089999,
0.42357609, 0.42625523, 0.42893746, 0.43162274, 0.43431118, 0.43700272,
0.43969744, 0.44239539, 0.44509652, 0.44780093, 0.45050862, 0.45321959,
0.45593393, 0.45865160, 0.46137270, 0.46409720, 0.46682513, 0.46955657,
0.47229150, 0.47502998, 0.47777200, 0.48051766, 0.48326692, 0.48601982,
0.48877645, 0.49153677, 0.49430084, 0.49706870, 0.49984038, 0.50261587,
0.50539529, 0.50817859, 0.51096582, 0.51375705, 0.51655227, 0.51935154,
0.52215493, 0.52496237, 0.52777398, 0.53058976, 0.53340971, 0.53623396,
0.53906250, 0.54189533, 0.54473257, 0.54757416, 0.55042017, 0.55327064,
0.55612564, 0.55898517, 0.56184924, 0.56471801, 0.56759137, 0.57046944,
0.57335222, 0.57623982, 0.57913220, 0.58202946, 0.58493161, 0.58783865,
0.59075069, 0.59366775, 0.59658992, 0.59951711, 0.60244948, 0.60538709,
0.60832989, 0.61127794, 0.61423135, 0.61719012, 0.62015426, 0.62312394,
0.62609905, 0.62907976, 0.63206607, 0.63505799, 0.63805562, 0.64105898,
0.64406818, 0.64708322, 0.65010411, 0.65313095, 0.65616381, 0.65920275,
0.66224772, 0.66529888, 0.66835624, 0.67141986, 0.67448980, 0.67756611,
0.68064880, 0.68373805, 0.68683380, 0.68993616, 0.69304514, 0.69616085,
0.69928336, 0.70241266, 0.70554888, 0.70869207, 0.71184224, 0.71499950,
0.71816391, 0.72133553, 0.72451442, 0.72770065, 0.73089427, 0.73409539,
0.73730403, 0.74052030, 0.74374419, 0.74697590, 0.75021541, 0.75346279,
0.75671816, 0.75998157, 0.76325303, 0.76653278, 0.76982075, 0.77311707,
0.77642179, 0.77973503, 0.78305680, 0.78638732, 0.78972650, 0.79307461,
0.79643154, 0.79979753, 0.80317259, 0.80655682, 0.80995029, 0.81335312,
0.81676543, 0.82018721, 0.82361871, 0.82705986, 0.83051085, 0.83397180,
0.83744276, 0.84092379, 0.84441507, 0.84791666, 0.85142869, 0.85495126,
0.85848445, 0.86202842, 0.86558324, 0.86914903, 0.87272590, 0.87631392,
0.87991333, 0.88352418, 0.88714653, 0.89078057, 0.89442647, 0.89808422,
0.90175408, 0.90543610, 0.90913045, 0.91283727, 0.91655666, 0.92028874,
0.92403370, 0.92779171, 0.93156284, 0.93534720, 0.93914509, 0.94295657,
0.94678175, 0.95062083, 0.95447403, 0.95834142, 0.96222317, 0.96611953,
0.97003061, 0.97395653, 0.97789752, 0.98185378, 0.98582548, 0.98981279,
0.99381590, 0.99783504, 1.0018703, 1.0059220, 1.0099902, 1.0140752,
1.0181773, 1.0222964, 1.0264331, 1.0305873, 1.0347595, 1.0389497,
1.0431583, 1.0473855, 1.0516313, 1.0558963, 1.0601805, 1.0644844,
1.0688080, 1.0731516, 1.0775156, 1.0819002, 1.0863057, 1.0907325,
1.0951807, 1.0996507, 1.1041428, 1.1086572, 1.1131943, 1.1177546,
1.1223381, 1.1269454, 1.1315767, 1.1362323, 1.1409128, 1.1456183,
1.1503494, 1.1551064, 1.1598897, 1.1646996, 1.1695366, 1.1744012,
1.1792938, 1.1842146, 1.1891644, 1.1941434, 1.1991522, 1.2041913,
1.2092612, 1.2143624, 1.2194953, 1.2246606, 1.2298588, 1.2350904,
1.2403560, 1.2456563, 1.2509917, 1.2563630, 1.2617708, 1.2672157,
1.2726985, 1.2782199, 1.2837805, 1.2893810, 1.2950224, 1.3007052,
1.3064302, 1.3121985, 1.3180108, 1.3238679, 1.3297709, 1.3357205,
1.3417178, 1.3477638, 1.3538593, 1.3600056, 1.3662038, 1.3724549,
1.3787601, 1.3851205, 1.3915375, 1.3980123, 1.4045463, 1.4111407,
1.4177971, 1.4245169, 1.4313017, 1.4381529, 1.4450725, 1.4520619,
1.4591230, 1.4662576, 1.4734676, 1.4807550, 1.4881219, 1.4955705,
1.5031030, 1.5107217, 1.5184292, 1.5262280, 1.5341206, 1.5421100,
1.5501990, 1.5583909, 1.5666887, 1.5750957, 1.5836155, 1.5922519,
1.6010087, 1.6098900, 1.6189002, 1.6280437, 1.6373254, 1.6467503,
1.6563238, 1.6660516, 1.6759397, 1.6859944, 1.6962224, 1.7066311,
1.7172281, 1.7280214, 1.7390199, 1.7502328, 1.7616704, 1.7733430,
1.7852623, 1.7974410, 1.8098922, 1.8226304, 1.8356714, 1.8490324,
1.8627318, 1.8767899, 1.8912292, 1.9060738, 1.9213507, 1.9370897,
1.9533237, 1.9700894, 1.9874278, 2.0053852, 2.0240135, 2.0433722,
2.0635278, 2.0845587, 2.1065540, 2.1296184, 2.1538746, 2.1794686,
2.2065752, 2.2354074, 2.2662268, 2.2993622, 2.3352330, 2.3743868,
2.4175589, 2.4657707, 2.5205021, 2.5840194, 2.6600673, 2.7555487,
2.8856349, 3.0972691, 2.4175589, 2.4182703, 2.4189830, 2.4196968,
2.4204118, 2.4211283, 2.4218459, 2.4225647, 2.4232848, 2.4240062,
2.4247286, 2.4254527, 2.4261777, 2.4269042, 2.4276319, 2.4283609,
2.4290912, 2.4298227, 2.4305556, 2.4312899, 2.4320254, 2.4327621,
2.4335003, 2.4342399, 2.4349806, 2.4357228, 2.4364665, 2.4372113,
2.4379575, 2.4387050, 2.4394538, 2.4402041, 2.4409559, 2.4417090,
2.4424634, 2.4432192, 2.4439764, 2.4447351, 2.4454951, 2.4462566,
2.4470196, 2.4477839, 2.4485495, 2.4493167, 2.4500854, 2.4508555,
2.4516270, 2.4524000, 2.4531746, 2.4539504, 2.4547279, 2.4555068,
2.4562871, 2.4570692, 2.4578526, 2.4586375, 2.4594238, 2.4602118,
2.4610014, 2.4617922, 2.4625850, 2.4633789, 2.4641747, 2.4649720,
2.4657707, 2.4665711, 2.4673731, 2.4681766, 2.4689817, 2.4697886,
2.4705970, 2.4714069, 2.4722185, 2.4730318, 2.4738467, 2.4746633,
2.4754813, 2.4763012, 2.4771228, 2.4779458, 2.4787707, 2.4795973,
2.4804256, 2.4812555, 2.4820871, 2.4829206, 2.4837558, 2.4845927,
2.4854312, 2.4862716, 2.4871137, 2.4879575, 2.4888031, 2.4896507,
2.4905000, 2.4913509, 2.4922037, 2.4930584, 2.4939148, 2.4947731,
2.4956334, 2.4964955, 2.4973593, 2.4982250, 2.4990926, 2.4999621,
2.5008335, 2.5017068, 2.5025821, 2.5034590, 2.5043380, 2.5052192,
2.5061021, 2.5069871, 2.5078738, 2.5087626, 2.5096536, 2.5105464,
2.5114412, 2.5123382, 2.5132370, 2.5141377, 2.5150409, 2.5159459,
2.5168529, 2.5177619, 2.5186732, 2.5195866, 2.5205021, 2.5214195,
2.5223393, 2.5232611, 2.5241852, 2.5251112, 2.5260396, 2.5269699,
2.5279026, 2.5288377, 2.5297747, 2.5307140, 2.5316556, 2.5325994,
2.5335455, 2.5344937, 2.5354445, 2.5363975, 2.5373526, 2.5383101,
2.5392702, 2.5402322, 2.5411968, 2.5421638, 2.5431333, 2.5441051,
2.5450790, 2.5460558, 2.5470347, 2.5480161, 2.5490000, 2.5499864,
2.5509753, 2.5519667, 2.5529606, 2.5539570, 2.5549560, 2.5559576,
2.5569615, 2.5579684, 2.5589776, 2.5599895, 2.5610039, 2.5620210,
2.5630410, 2.5640633, 2.5650885, 2.5661166, 2.5671470, 2.5681803,
2.5692165, 2.5702553, 2.5712969, 2.5723414, 2.5733886, 2.5744386,
2.5754917, 2.5765474, 2.5776060, 2.5786676, 2.5797319, 2.5807993,
2.5818698, 2.5829432, 2.5840194, 2.5850987, 2.5861809, 2.5872664,
2.5883548, 2.5894463, 2.5905409, 2.5916386, 2.5927393, 2.5938435,
2.5949504, 2.5960608, 2.5971744, 2.5982912, 2.5994112, 2.6005344,
2.6016610, 2.6027911, 2.6039243, 2.6050608, 2.6062007, 2.6073442,
2.6084909, 2.6096411, 2.6107948, 2.6119521, 2.6131127, 2.6142769,
2.6154447, 2.6166160, 2.6177909, 2.6189694, 2.6201518, 2.6213377,
2.6225271, 2.6237204, 2.6249175, 2.6261184, 2.6273229, 2.6285315,
2.6297438, 2.6309600, 2.6321800, 2.6334040, 2.6346319, 2.6358640,
2.6371000, 2.6383400, 2.6395841, 2.6408322, 2.6420846, 2.6433411,
2.6446016, 2.6458664, 2.6471355, 2.6484089, 2.6496868, 2.6509688,
2.6522551, 2.6535461, 2.6548412, 2.6561410, 2.6574452, 2.6587539,
2.6600673, 2.6613853, 2.6627078, 2.6640351, 2.6653671, 2.6667037,
2.6680453, 2.6693916, 2.6707428, 2.6720989, 2.6734598, 2.6748257,
2.6761968, 2.6775727, 2.6789539, 2.6803401, 2.6817315, 2.6831281,
2.6845300, 2.6859372, 2.6873498, 2.6887677, 2.6901908, 2.6916196,
2.6930540, 2.6944938, 2.6959393, 2.6973903, 2.6988473, 2.7003098,
2.7017782, 2.7032526, 2.7047327, 2.7062187, 2.7077107, 2.7092090,
2.7107131, 2.7122235, 2.7137401, 2.7152629, 2.7167921, 2.7183278,
2.7198699, 2.7214181, 2.7229733, 2.7245350, 2.7261033, 2.7276783,
2.7292602, 2.7308490, 2.7324448, 2.7340474, 2.7356570, 2.7372739,
2.7388978, 2.7405291, 2.7421677, 2.7438138, 2.7454672, 2.7471282,
2.7487967, 2.7504730, 2.7521570, 2.7538490, 2.7555487, 2.7572565,
2.7589726, 2.7606966, 2.7624290, 2.7641695, 2.7659185, 2.7676761,
2.7694423, 2.7712171, 2.7730007, 2.7747931, 2.7765946, 2.7784052,
2.7802248, 2.7820535, 2.7838919, 2.7857394, 2.7875967, 2.7894638,
2.7913404, 2.7932270, 2.7951236, 2.7970302, 2.7989471, 2.8008742,
2.8028121, 2.8047602, 2.8067193, 2.8086891, 2.8106699, 2.8126616,
2.8146648, 2.8166792, 2.8187053, 2.8207428, 2.8227921, 2.8248534,
2.8269267, 2.8290124, 2.8311102, 2.8332207, 2.8353441, 2.8374801,
2.8396292, 2.8417914, 2.8439670, 2.8461561, 2.8483591, 2.8505759,
2.8528068, 2.8550520, 2.8573117, 2.8595860, 2.8618753, 2.8641796,
2.8664994, 2.8688345, 2.8711855, 2.8735523, 2.8759356, 2.8783350,
2.8807514, 2.8831847, 2.8856349, 2.8881028, 2.8905883, 2.8930919,
2.8956137, 2.8981540, 2.9007132, 2.9032917, 2.9058893, 2.9085069,
2.9111445, 2.9138026, 2.9164813, 2.9191813, 2.9219027, 2.9246457,
2.9274111, 2.9301989, 2.9330099, 2.9358439, 2.9387021, 2.9415841,
2.9444911, 2.9474230, 2.9503806, 2.9533641, 2.9563742, 2.9594111,
2.9624758, 2.9655685, 2.9686899, 2.9718404, 2.9750206, 2.9782314,
2.9814732, 2.9847465, 2.9880521, 2.9913907, 2.9947629, 2.9981697,
3.0016115, 3.0050893, 3.0086038, 3.0121558, 3.0157464, 3.0193760,
3.0230460, 3.0267572, 3.0305107, 3.0343070, 3.0381479, 3.0420339,
3.0459666, 3.0499468, 3.0539761, 3.0580554, 3.0621862, 3.0663702,
3.0706084, 3.0749025, 3.0792542, 3.0836647, 3.0881362, 3.0926704,
3.0972691, 3.1019340, 3.1066675, 3.1114717, 3.1163487, 3.1213012,
3.1263313, 3.1314418, 3.1366353, 3.1419148, 3.1472836, 3.1527445,
3.1583011, 3.1639571, 3.1697159, 3.1755819, 3.1815593, 3.1876526,
3.1938665, 3.2002063, 3.2066774, 3.2132857, 3.2200372, 3.2269390,
3.2339978, 3.2412214, 3.2486186, 3.2561977, 3.2639687, 3.2719417,
3.2801285, 3.2885413, 3.2971933, 3.3060997, 3.3152759, 3.3247404,
3.3345122, 3.3446131, 3.3550673, 3.3659017, 3.3771460, 3.3888345,
3.4010048, 3.4137008, 3.4269724, 3.4408762, 3.4554789, 3.4708579,
3.4871042, 3.5043266, 3.5226557, 3.5422506, 3.5633078, 3.5860744,
3.6108665, 3.6380987, 3.6683295, 3.7023365, 3.7412527, 3.7868195,
3.8419309, 3.9119532, 4.0087729, 4.1695695, 3.6683295, 3.6688292,
3.6693301, 3.6698320, 3.6703346, 3.6708381, 3.6713428, 3.6718483,
3.6723547, 3.6728621, 3.6733704, 3.6738796, 3.6743898, 3.6749010,
3.6754131, 3.6759262, 3.6764402, 3.6769555, 3.6774714, 3.6779885,
3.6785064, 3.6790254, 3.6795454, 3.6800663, 3.6805882, 3.6811113,
3.6816354, 3.6821604, 3.6826863, 3.6832132, 3.6837413, 3.6842704,
3.6848006, 3.6853318, 3.6858640, 3.6863973, 3.6869316, 3.6874669,
3.6880033, 3.6885407, 3.6890793, 3.6896188, 3.6901596, 3.6907015,
3.6912444, 3.6917882, 3.6923332, 3.6928794, 3.6934266, 3.6939750,
3.6945245, 3.6950750, 3.6956267, 3.6961796, 3.6967335, 3.6972888,
3.6978450, 3.6984024, 3.6989608, 3.6995206, 3.7000813, 3.7006435,
3.7012067, 3.7017710, 3.7023365, 3.7029033, 3.7034712, 3.7040403,
3.7046108, 3.7051823, 3.7057550, 3.7063291, 3.7069042, 3.7074807,
3.7080584, 3.7086372, 3.7092173, 3.7097988, 3.7103813, 3.7109654,
3.7115505, 3.7121370, 3.7127247, 3.7133138, 3.7139039, 3.7144957,
3.7150886, 3.7156827, 3.7162783, 3.7168751, 3.7174733, 3.7180729,
3.7186737, 3.7192760, 3.7198794, 3.7204845, 3.7210908, 3.7216983,
3.7223074, 3.7229178, 3.7235296, 3.7241428, 3.7247574, 3.7253735,
3.7259908, 3.7266097, 3.7272301, 3.7278516, 3.7284749, 3.7290995,
3.7297256, 3.7303531, 3.7309821, 3.7316127, 3.7322445, 3.7328780,
3.7335131, 3.7341495, 3.7347875, 3.7354269, 3.7360680, 3.7367105,
3.7373548, 3.7380004, 3.7386477, 3.7392967, 3.7399471, 3.7405989,
3.7412527, 3.7419078, 3.7425644, 3.7432230, 3.7438829, 3.7445447,
3.7452080, 3.7458730, 3.7465396, 3.7472079, 3.7478778, 3.7485495,
3.7492230, 3.7498980, 3.7505748, 3.7512534, 3.7519336, 3.7526155,
3.7532992, 3.7539847, 3.7546721, 3.7553611, 3.7560518, 3.7567444,
3.7574389, 3.7581351, 3.7588332, 3.7595329, 3.7602348, 3.7609384,
3.7616439, 3.7623513, 3.7630606, 3.7637715, 3.7644846, 3.7651997,
3.7659166, 3.7666354, 3.7673562, 3.7680788, 3.7688036, 3.7695303,
3.7702589, 3.7709897, 3.7717223, 3.7724571, 3.7731941, 3.7739329,
3.7746737, 3.7754169, 3.7761619, 3.7769091, 3.7776582, 3.7784097,
3.7791634, 3.7799191, 3.7806768, 3.7814369, 3.7821991, 3.7829638,
3.7837305, 3.7844994, 3.7852705, 3.7860439, 3.7868195, 3.7875974,
3.7883778, 3.7891603, 3.7899454, 3.7907326, 3.7915220, 3.7923141,
3.7931085, 3.7939053, 3.7947044, 3.7955060, 3.7963102, 3.7971165,
3.7979257, 3.7987370, 3.7995510, 3.8003676, 3.8011866, 3.8020084,
3.8028326, 3.8036592, 3.8044887, 3.8053207, 3.8061554, 3.8069928,
3.8078327, 3.8086753, 3.8095207, 3.8103690, 3.8112199, 3.8120735,
3.8129299, 3.8137891, 3.8146513, 3.8155162, 3.8163841, 3.8172548,
3.8181283, 3.8190048, 3.8198843, 3.8207667, 3.8216519, 3.8225403,
3.8234317, 3.8243263, 3.8252239, 3.8261244, 3.8270283, 3.8279352,
3.8288453, 3.8297584, 3.8306749, 3.8315945, 3.8325174, 3.8334436,
3.8343730, 3.8353059, 3.8362420, 3.8371816, 3.8381245, 3.8390708,
3.8400207, 3.8409741, 3.8419309, 3.8428912, 3.8438549, 3.8448224,
3.8457935, 3.8467684, 3.8477466, 3.8487287, 3.8497145, 3.8507042,
3.8516974, 3.8526945, 3.8536956, 3.8547006, 3.8557093, 3.8567221,
3.8577387, 3.8587594, 3.8597841, 3.8608131, 3.8618460, 3.8628831,
3.8639243, 3.8649697, 3.8660192, 3.8670733, 3.8681316, 3.8691943,
3.8702612, 3.8713326, 3.8724084, 3.8734889, 3.8745737, 3.8756633,
3.8767574, 3.8778563, 3.8789597, 3.8800678, 3.8811810, 3.8822987,
3.8834214, 3.8845491, 3.8856819, 3.8868194, 3.8879621, 3.8891098,
3.8902626, 3.8914208, 3.8925841, 3.8937528, 3.8949268, 3.8961060,
3.8972909, 3.8984811, 3.8996770, 3.9008787, 3.9020858, 3.9032986,
3.9045172, 3.9057415, 3.9069719, 3.9082081, 3.9094503, 3.9106987,
3.9119532, 3.9132137, 3.9144807, 3.9157538, 3.9170334, 3.9183195,
3.9196119, 3.9209111, 3.9222169, 3.9235291, 3.9248486, 3.9261746,
3.9275079, 3.9288478, 3.9301951, 3.9315493, 3.9329109, 3.9342799,
3.9356563, 3.9370401, 3.9384317, 3.9398308, 3.9412377, 3.9426522,
3.9440749, 3.9455056, 3.9469445, 3.9483914, 3.9498467, 3.9513104,
3.9527826, 3.9542634, 3.9557531, 3.9572515, 3.9587588, 3.9602754,
3.9618008, 3.9633358, 3.9648800, 3.9664338, 3.9679971, 3.9695702,
3.9711533, 3.9727464, 3.9743495, 3.9759629, 3.9775867, 3.9792213,
3.9808664, 3.9825225, 3.9841895, 3.9858675, 3.9875569, 3.9892578,
3.9909704, 3.9926946, 3.9944310, 3.9961793, 3.9979401, 3.9997132,
4.0014987, 4.0032978, 4.0051093, 4.0069342, 4.0087729, 4.0106249,
4.0124907, 4.0143704, 4.0162649, 4.0181737, 4.0200973, 4.0220361,
4.0239897, 4.0259590, 4.0279441, 4.0299449, 4.0319624, 4.0339961,
4.0360465, 4.0381145, 4.0401998, 4.0423026, 4.0444231, 4.0465627,
4.0487204, 4.0508976, 4.0530939, 4.0553098, 4.0575461, 4.0598025,
4.0620799, 4.0643787, 4.0666995, 4.0690417, 4.0714068, 4.0737948,
4.0762067, 4.0786419, 4.0811019, 4.0835867, 4.0860972, 4.0886335,
4.0911965, 4.0937867, 4.0964046, 4.0990505, 4.1017261, 4.1044307,
4.1071663, 4.1099324, 4.1127305, 4.1155615, 4.1184258, 4.1213241,
4.1242576, 4.1272268, 4.1302328, 4.1332765, 4.1363592, 4.1394820,
4.1426454, 4.1458507, 4.1490993, 4.1523924, 4.1557312, 4.1591167,
4.1625504, 4.1660342, 4.1695695, 4.1731572, 4.1767998, 4.1804986,
4.1842556, 4.1880722, 4.1919513, 4.1958942, 4.1999035, 4.2039814,
4.2081308, 4.2123537, 4.2166529, 4.2210317, 4.2254925, 4.2300396,
4.2346759, 4.2394047, 4.2442303, 4.2491570, 4.2541885, 4.2593307,
4.2645879, 4.2699656, 4.2754698, 4.2811065, 4.2868824, 4.2928052,
4.2988825, 4.3051229, 4.3115358, 4.3181305, 4.3249192, 4.3319130,
4.3391252, 4.3465705, 4.3542647, 4.3622255, 4.3704729, 4.3790288,
4.3879180, 4.3971677, 4.4068098, 4.4168801, 4.4274187, 4.4384737,
4.4500990, 4.4623590, 4.4753284, 4.4890976, 4.5037742, 4.5194898,
4.5364079, 4.5547333, 4.5747290, 4.5967398, 4.6212311, 4.6488533,
4.6805530, 4.7177916, 4.7630010, 4.8207059, 4.9009643, 5.0354061,
4.6212311, 4.6216364, 4.6220427, 4.6224499, 4.6228576, 4.6232662,
4.6236758, 4.6240859, 4.6244965, 4.6249084, 4.6253209, 4.6257339,
4.6261482, 4.6265631, 4.6269784, 4.6273952, 4.6278124, 4.6282306,
4.6286492, 4.6290689, 4.6294894, 4.6299109, 4.6303329, 4.6307559,
4.6311798, 4.6316042, 4.6320295, 4.6324563, 4.6328831, 4.6333113,
4.6337399, 4.6341696, 4.6346002, 4.6350317, 4.6354642, 4.6358972,
4.6363311, 4.6367660, 4.6372018, 4.6376386, 4.6380763, 4.6385145,
4.6389537, 4.6393943, 4.6398354, 4.6402774, 4.6407204, 4.6411643,
4.6416087, 4.6420546, 4.6425014, 4.6429486, 4.6433973, 4.6438465,
4.6442971, 4.6447482, 4.6452007, 4.6456537, 4.6461077, 4.6465631,
4.6470189, 4.6474762, 4.6479340, 4.6483932, 4.6488533, 4.6493139,
4.6497760, 4.6502390, 4.6507030, 4.6511679, 4.6516337, 4.6521006,
4.6525688, 4.6530375, 4.6535077, 4.6539788, 4.6544509, 4.6549239,
4.6553984, 4.6558733, 4.6563497, 4.6568270, 4.6573057, 4.6577849,
4.6582656, 4.6587472, 4.6592298, 4.6597137, 4.6601987, 4.6606846,
4.6611714, 4.6616597, 4.6621490, 4.6626396, 4.6631312, 4.6636238,
4.6641178, 4.6646128, 4.6651087, 4.6656060, 4.6661043, 4.6666040,
4.6671047, 4.6676068, 4.6681099, 4.6686139, 4.6691194, 4.6696262,
4.6701341, 4.6706429, 4.6711535, 4.6716647, 4.6721778, 4.6726913,
4.6732068, 4.6737232, 4.6742411, 4.6747599, 4.6752801, 4.6758018,
4.6763244, 4.6768484, 4.6773734, 4.6779003, 4.6784282, 4.6789575,
4.6794877, 4.6800199, 4.6805530, 4.6810875, 4.6816235, 4.6821604,
4.6826992, 4.6832390, 4.6837802, 4.6843228, 4.6848669, 4.6854124,
4.6859593, 4.6865077, 4.6870570, 4.6876082, 4.6881609, 4.6887150,
4.6892700, 4.6898270, 4.6903853, 4.6909451, 4.6915064, 4.6920691,
4.6926332, 4.6931987, 4.6937661, 4.6943350, 4.6949048, 4.6954765,
4.6960502, 4.6966248, 4.6972013, 4.6977792, 4.6983590, 4.6989398,
4.6995225, 4.7001071, 4.7006931, 4.7012806, 4.7018695, 4.7024608,
4.7030530, 4.7036471, 4.7042427, 4.7048402, 4.7054396, 4.7060404,
4.7066431, 4.7072473, 4.7078533, 4.7084608, 4.7090702, 4.7096815,
4.7102942, 4.7109094, 4.7115259, 4.7121439, 4.7127643, 4.7133861,
4.7140098, 4.7146354, 4.7152629, 4.7158923, 4.7165236, 4.7171569,
4.7177916, 4.7184286, 4.7190676, 4.7197080, 4.7203507, 4.7209954,
4.7216420, 4.7222905, 4.7229414, 4.7235937, 4.7242484, 4.7249050,
4.7255640, 4.7262244, 4.7268872, 4.7275524, 4.7282190, 4.7288885,
4.7295594, 4.7302327, 4.7309084, 4.7315860, 4.7322659, 4.7329478,
4.7336321, 4.7343187, 4.7350078, 4.7356987, 4.7363915, 4.7370872,
4.7377853, 4.7384853, 4.7391877, 4.7398925, 4.7405996, 4.7413092,
4.7420211, 4.7427359, 4.7434525, 4.7441716, 4.7448936, 4.7456174,
4.7463441, 4.7470737, 4.7478051, 4.7485394, 4.7492762, 4.7500157,
4.7507577, 4.7515025, 4.7522497, 4.7529998, 4.7537527, 4.7545080,
4.7552657, 4.7560267, 4.7567902, 4.7575569, 4.7583261, 4.7590981,
4.7598729, 4.7606506, 4.7614312, 4.7622147, 4.7630010, 4.7637906,
4.7645831, 4.7653785, 4.7661767, 4.7669783, 4.7677827, 4.7685905,
4.7694011, 4.7702150, 4.7710323, 4.7718525, 4.7726760, 4.7735028,
4.7743330, 4.7751660, 4.7760029, 4.7768431, 4.7776861, 4.7785330,
4.7793832, 4.7802372, 4.7810946, 4.7819552, 4.7828193, 4.7836871,
4.7845588, 4.7854342, 4.7863131, 4.7871957, 4.7880816, 4.7889719,
4.7898660, 4.7907634, 4.7916651, 4.7925706, 4.7934799, 4.7943935,
4.7953110, 4.7962327, 4.7971582, 4.7980881, 4.7990217, 4.7999601,
4.8009024, 4.8018489, 4.8028002, 4.8037553, 4.8047152, 4.8056793,
4.8066478, 4.8076210, 4.8085990, 4.8095813, 4.8105683, 4.8115602,
4.8125567, 4.8135581, 4.8145642, 4.8155751, 4.8165913, 4.8176122,
4.8186383, 4.8196697, 4.8207059, 4.8217473, 4.8227944, 4.8238463,
4.8249040, 4.8259668, 4.8270350, 4.8281088, 4.8291883, 4.8302736,
4.8313646, 4.8324614, 4.8335638, 4.8346720, 4.8357863, 4.8369069,
4.8380332, 4.8391662, 4.8403049, 4.8414502, 4.8426018, 4.8437600,
4.8449244, 4.8460956, 4.8472738, 4.8484583, 4.8496499, 4.8508482,
4.8520536, 4.8532662, 4.8544860, 4.8557129, 4.8569474, 4.8581891,
4.8594384, 4.8606954, 4.8619604, 4.8632326, 4.8645129, 4.8658018,
4.8670983, 4.8684030, 4.8697162, 4.8710380, 4.8723679, 4.8737068,
4.8750544, 4.8764110, 4.8777766, 4.8791513, 4.8805356, 4.8819289,
4.8833318, 4.8847446, 4.8861670, 4.8875995, 4.8890419, 4.8904948,
4.8919582, 4.8934317, 4.8949161, 4.8964114, 4.8979177, 4.8994355,
4.9009643, 4.9025049, 4.9040570, 4.9056211, 4.9071970, 4.9087858,
4.9103866, 4.9120002, 4.9136267, 4.9152665, 4.9169192, 4.9185858,
4.9202662, 4.9219604, 4.9236689, 4.9253917, 4.9271293, 4.9288821,
4.9306502, 4.9324336, 4.9342332, 4.9360490, 4.9378805, 4.9397292,
4.9415951, 4.9434781, 4.9453788, 4.9472976, 4.9492350, 4.9511909,
4.9531660, 4.9551606, 4.9571748, 4.9592099, 4.9612656, 4.9633422,
4.9654403, 4.9675608, 4.9697042, 4.9718699, 4.9740596, 4.9762735,
4.9785118, 4.9807758, 4.9830651, 4.9853811, 4.9877243, 4.9900947,
4.9924936, 4.9949222, 4.9973798, 4.9998684, 5.0023885, 5.0049405,
5.0075254, 5.0101442, 5.0127983, 5.0154877, 5.0182142, 5.0209785,
5.0237813, 5.0266242, 5.0295086, 5.0324354, 5.0354061, 5.0384216,
5.0414839, 5.0445938, 5.0477538, 5.0509648, 5.0542288, 5.0575476,
5.0609226, 5.0643568, 5.0678515, 5.0714092, 5.0750322, 5.0787234,
5.0824847, 5.0863199, 5.0902309, 5.0942216, 5.0982947, 5.1024547,
5.1067047, 5.1110492, 5.1154919, 5.1200380, 5.1246929, 5.1294613,
5.1343493, 5.1393628, 5.1445093, 5.1497955, 5.1552300, 5.1608210,
5.1665783, 5.1725116, 5.1786332, 5.1849551, 5.1914916, 5.1982570,
5.2052698, 5.2125478, 5.2201133, 5.2279892, 5.2362041, 5.2447882,
5.2537766, 5.2632108, 5.2731380, 5.2836142, 5.2947040, 5.3064861,
5.3190541, 5.3325229, 5.3470345, 5.3627677, 5.3799520, 5.3988881,
5.4199834, 5.4438057, 5.4711857, 5.5034032, 5.5425940, 5.5927372,
5.6626978, 5.7804394
};
    static double lpgscal[4];
    unsigned long  index1, index2;
    int i;
    static char firstcall = 1;
    double u, y;
	long rn;
    unsigned long *x = (unsigned long *)&rn;
    
    if(firstcall) {
		firstcall = 0;
		for(i = 0; i < 4; i++) {
		    /* scale factor for table lookup */
		    lpgscal[i] = (double)ldexp(1.0, (6*i-22));
		}
	}

	/* loop twice the number of samples because complex */
    for (i = 0; i < 2 * nsamples; i++) {
		/* below are man page excerps:
		 * 'mrand48() and jrand48() return  signed  long  integers  uni-
		 * formly distributed over the interval [-2**31 ~2**31).
		 * erand48(), nrand48(), and jrand48() do not require an initialization
		 * entry point to be called first.'
		 */
		rn = mrand48();
                /*printf("CGAUSS48: rn is: %d\n",rn);*/

		index1 = (*x >> 22) & KEEP_9LSB_MASK;    /* get integer portion*/
		if ((index1 >> 3) != 63) { /* first 6 bits of index1 are not all ones */
			/* get fractional portion of index*/
			index2 = *x & CLEAR_10MSB_MASK; 
			u = lpgscal[0] * (float) index2;/* scale to be (0.,1.)*/
			y = (double)gausstbl[index1]+
				u*(double)(gausstbl[index1+1]-gausstbl[index1]);
			if ((*x & SIGNMASK) != 0) y =- y;
			goto next;
		}
	
		index1 = (*x >> 16) & KEEP_9LSB_MASK;/* get integer portion*/
		if ((index1 >> 3) != 63) { /* first 6 bits of index1 are not all ones*/
			index2 = *x & CLEAR_16MSB_MASK; /* get fractional portion of index*/
			u = lpgscal[1] * (double) index2;/* scale to be (0.,1.)*/
			y = (double)gausstbl[index1+512]+
			u*(double)(gausstbl[index1+513] - gausstbl[index1+512]);
			if ((*x & SIGNMASK) != 0) y =- y;
			goto next;
		}
	
		index1 = (*x >> 10) & KEEP_9LSB_MASK;/* get integer portion*/
		if ((index1 >> 3) != 63) { /* first 6 bits of index1 are not all ones */
			index2 = *x & CLEAR_22MSB_MASK; /* get fractional portion of index*/
			u = lpgscal[2] * (double) index2;/* scale to be (0.,1.)*/

			y = (double)gausstbl[index1+1024]+u*(double)(gausstbl[index1+1025] -
				gausstbl[index1+1024]);
			if ((*x & SIGNMASK) != 0) y =- y;
			goto next;
		}
		index1 = (*x >> 4) & KEEP_9LSB_MASK;/* get integer portion*/
		if (index1 != 511) {/*if 9 bits of index1 are not all ones*/
			index2 = *x & CLEAR_28MSB_MASK; /* get fractional portion of index*/
			u = lpgscal[3] * (double) index2;/* scale to be (0.,1.)*/
			y = (double)gausstbl[index1+1536]+
			u*(double)(gausstbl[index1+1537] - gausstbl[index1+1536]);
			if ((*x & SIGNMASK) != 0) y =- y;
			goto next;
		}
	
		y = (double)gausstbl[index1+1536];
		if ((*x & SIGNMASK) == 1) y = -y;
		
		next:
		if (y > 100. || y < -100.) {
		  printf ("CGAUSS48 Error: x = %lx, y = %lf\n", *x, y);
		}

		/* divide real and image parts by sqrt(2) because complex */
		data[i] = (float) (y/SQRT2);
	}
}
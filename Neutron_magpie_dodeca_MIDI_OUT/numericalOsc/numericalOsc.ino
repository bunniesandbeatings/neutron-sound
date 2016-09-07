/*
   Neutron magpie dodeca sketch:
   NCO. it is an oscillator that does not use a phase accumulator, but changes the sample rate instead.
   3 voice poly (no decay ot voice stealing yet) this could be improved with a built in decay and note stealing. 

   So far, there can only be 3 note polyphony with NCO oscillators because it uses the 3 timers that are used for PWM pins, because you can specify 
   the actual frequency.
   
   Waveform is output on 2, gate on 4.

 * */

#include <MIDI.h>

const int16_t sinTable[] = {
  0, 402, 804, 1206, 1607, 2009, 2410, 2811, 3211, 3611, 4011, 4409, 4807, 5205, 5601, 5997, 6392,
  6786, 7179, 7571, 7961, 8351, 8739, 9126, 9511, 9895, 10278, 10659, 11038, 11416, 11792, 12166, 12539,
  12909, 13278, 13645, 14009, 14372, 14732, 15090, 15446, 15799, 16150, 16499, 16845, 17189, 17530, 17868, 18204,
  18537, 18867, 19194, 19519, 19840, 20159, 20474, 20787, 21096, 21402, 21705, 22004, 22301, 22594, 22883, 23169,
  23452, 23731, 24006, 24278, 24546, 24811, 25072, 25329, 25582, 25831, 26077, 26318, 26556, 26789, 27019, 27244,
  27466, 27683, 27896, 28105, 28309, 28510, 28706, 28897, 29085, 29268, 29446, 29621, 29790, 29955, 30116, 30272,
  30424, 30571, 30713, 30851, 30984, 31113, 31236, 31356, 31470, 31580, 31684, 31785, 31880, 31970, 32056, 32137,
  32213, 32284, 32350, 32412, 32468, 32520, 32567, 32609, 32646, 32678, 32705, 32727, 32744, 32757, 32764, 32767,
  32764, 32757, 32744, 32727, 32705, 32678, 32646, 32609, 32567, 32520, 32468, 32412, 32350, 32284, 32213, 32137,
  32056, 31970, 31880, 31785, 31684, 31580, 31470, 31356, 31236, 31113, 30984, 30851, 30713, 30571, 30424, 30272,
  30116, 29955, 29790, 29621, 29446, 29268, 29085, 28897, 28706, 28510, 28309, 28105, 27896, 27683, 27466, 27244,
  27019, 26789, 26556, 26318, 26077, 25831, 25582, 25329, 25072, 24811, 24546, 24278, 24006, 23731, 23452, 23169,
  22883, 22594, 22301, 22004, 21705, 21402, 21096, 20787, 20474, 20159, 19840, 19519, 19194, 18867, 18537, 18204,
  17868, 17530, 17189, 16845, 16499, 16150, 15799, 15446, 15090, 14732, 14372, 14009, 13645, 13278, 12909, 12539,
  12166, 11792, 11416, 11038, 10659, 10278, 9895, 9511, 9126, 8739, 8351, 7961, 7571, 7179, 6786, 6392,
  5997, 5601, 5205, 4807, 4409, 4011, 3611, 3211, 2811, 2410, 2009, 1607, 1206, 804, 402, 0,
  -402, -804, -1206, -1607, -2009, -2410, -2811, -3211, -3611, -4011, -4409, -4807, -5205, -5601, -5997, -6392,
  -6786, -7179, -7571, -7961, -8351, -8739, -9126, -9511, -9895, -10278, -10659, -11038, -11416, -11792, -12166, -12539,
  -12909, -13278, -13645, -14009, -14372, -14732, -15090, -15446, -15799, -16150, -16499, -16845, -17189, -17530, -17868, -18204,
  -18537, -18867, -19194, -19519, -19840, -20159, -20474, -20787, -21096, -21402, -21705, -22004, -22301, -22594, -22883, -23169,
  -23452, -23731, -24006, -24278, -24546, -24811, -25072, -25329, -25582, -25831, -26077, -26318, -26556, -26789, -27019, -27244,
  -27466, -27683, -27896, -28105, -28309, -28510, -28706, -28897, -29085, -29268, -29446, -29621, -29790, -29955, -30116, -30272,
  -30424, -30571, -30713, -30851, -30984, -31113, -31236, -31356, -31470, -31580, -31684, -31785, -31880, -31970, -32056, -32137,
  -32213, -32284, -32350, -32412, -32468, -32520, -32567, -32609, -32646, -32678, -32705, -32727, -32744, -32757, -32764, -32767,
  -32764, -32757, -32744, -32727, -32705, -32678, -32646, -32609, -32567, -32520, -32468, -32412, -32350, -32284, -32213, -32137,
  -32056, -31970, -31880, -31785, -31684, -31580, -31470, -31356, -31236, -31113, -30984, -30851, -30713, -30571, -30424, -30272,
  -30116, -29955, -29790, -29621, -29446, -29268, -29085, -28897, -28706, -28510, -28309, -28105, -27896, -27683, -27466, -27244,
  -27019, -26789, -26556, -26318, -26077, -25831, -25582, -25329, -25072, -24811, -24546, -24278, -24006, -23731, -23452, -23169,
  -22883, -22594, -22301, -22004, -21705, -21402, -21096, -20787, -20474, -20159, -19840, -19519, -19194, -18867, -18537, -18204,
  -17868, -17530, -17189, -16845, -16499, -16150, -15799, -15446, -15090, -14732, -14372, -14009, -13645, -13278, -12909, -12539,
  -12166, -11792, -11416, -11038, -10659, -10278, -9895, -9511, -9126, -8739, -8351, -7961, -7571, -7179, -6786, -6392,
  -5997, -5601, -5205, -4807, -4409, -4011, -3611, -3211, -2811, -2410, -2009, -1607, -1206, -804, -402,
};


const int16_t triTable[] = {  
    -32511, -32255, -31998, -31743, -31487, -31231, -30974, -30719, -30463, -30207, -29951, -29695, -29439, -29183, -28927, -28671,
  -28415, -28159, -27903, -27647, -27391, -27135, -26879, -26623, -26367, -26111, -25855, -25599, -25343, -25087, -24831, -24575,
  -24319, -24063, -23807, -23551, -23295, -23039, -22783, -22527, -22271, -22015, -21759, -21503, -21247, -20991, -20735, -20479,
  -20223, -19967, -19711, -19455, -19199, -18943, -18687, -18431, -18175, -17919, -17663, -17407, -17151, -16895, -16639, -16383,
  -16127, -15871, -15615, -15359, -15103, -14847, -14591, -14335, -14079, -13823, -13567, -13311, -13055, -12799, -12543, -12287,
  -12031, -11775, -11519, -11263, -11007, -10751, -10495, -10239, -9983, -9727, -9471, -9215, -8959, -8703, -8447, -8191,
  -7935, -7679, -7423, -7167, -6911, -6655, -6399, -6143, -5887, -5631, -5375, -5119, -4863, -4608, -4351, -4095,
  -3839, -3584, -3327, -3071, -2815, -2559, -2303, -2047, -1791, -1535, -1279, -1023, -767, -511, -255, 0,
  255, 511, 767, 1023, 1279, 1535, 1791, 2047, 2303, 2559, 2815, 3071, 3327, 3584, 3839, 4095,
  4351, 4608, 4863, 5119, 5375, 5631, 5887, 6143, 6399, 6655, 6911, 7167, 7423, 7679, 7935, 8191,
  8447, 8703, 8959, 9215, 9471, 9727, 9983, 10239, 10495, 10751, 11007, 11263, 11519, 11775, 12031, 12287,
  12543, 12799, 13055, 13311, 13567, 13823, 14079, 14335, 14591, 14847, 15103, 15359, 15615, 15871, 16127, 16383,
  16639, 16895, 17151, 17407, 17663, 17919, 18175, 18431, 18687, 18943, 19199, 19455, 19711, 19967, 20223, 20479,
  20735, 20991, 21247, 21503, 21759, 22015, 22271, 22527, 22783, 23039, 23295, 23551, 23807, 24063, 24319, 24575,
  24831, 25087, 25343, 25599, 25855, 26111, 26367, 26623, 26879, 27135, 27391, 27647, 27903, 28159, 28415, 28671,
  28927, 29183, 29439, 29695, 29951, 30207, 30463, 30719, 30974, 31231, 31487, 31743, 31998, 32255, 32511, 32767,
  32511, 32255, 31998, 31743, 31487, 31231, 30974, 30719, 30463, 30207, 29951, 29695, 29439, 29183, 28927, 28671,
  28415, 28159, 27903, 27647, 27391, 27135, 26879, 26623, 26367, 26111, 25855, 25599, 25343, 25087, 24831, 24575,
  24319, 24063, 23807, 23551, 23295, 23039, 22783, 22527, 22271, 22015, 21759, 21503, 21247, 20991, 20735, 20479,
  20223, 19967, 19711, 19455, 19199, 18943, 18687, 18431, 18175, 17919, 17663, 17407, 17151, 16895, 16639, 16383,
  16127, 15871, 15615, 15359, 15103, 14847, 14591, 14335, 14079, 13823, 13567, 13311, 13055, 12799, 12543, 12287,
  12031, 11775, 11519, 11263, 11007, 10751, 10495, 10239, 9983, 9727, 9471, 9215, 8959, 8703, 8447, 8191,
  7935, 7679, 7423, 7167, 6911, 6655, 6399, 6143, 5887, 5631, 5375, 5119, 4863, 4608, 4351, 4095,
  3839, 3584, 3327, 3071, 2815, 2559, 2303, 2047, 1791, 1535, 1279, 1023, 767, 511, 255, 0,
  -255, -511, -767, -1023, -1279, -1535, -1791, -2047, -2303, -2559, -2815, -3071, -3327, -3584, -3839, -4095,
  -4351, -4608, -4863, -5119, -5375, -5631, -5887, -6143, -6399, -6655, -6911, -7167, -7423, -7679, -7935, -8191,
  -8447, -8703, -8959, -9215, -9471, -9727, -9983, -10239, -10495, -10751, -11007, -11263, -11519, -11775, -12031, -12287,
  -12543, -12799, -13055, -13311, -13567, -13823, -14079, -14335, -14591, -14847, -15103, -15359, -15615, -15871, -16127, -16383,
  -16639, -16895, -17151, -17407, -17663, -17919, -18175, -18431, -18687, -18943, -19199, -19455, -19711, -19967, -20223, -20479,
  -20735, -20991, -21247, -21503, -21759, -22015, -22271, -22527, -22783, -23039, -23295, -23551, -23807, -24063, -24319, -24575,
  -24831, -25087, -25343, -25599, -25855, -26111, -26367, -26623, -26879, -27135, -27391, -27647, -27903, -28159, -28415, -28671,
  -28927, -29183, -29439, -29695, -29951, -30207, -30463, -30719, -30974, -31231, -31487, -31743, -31998, -32255, -32511, -32767,
};

const int16_t sawTable[] = {
  32640, 32512, 32384, 32256, 32128, 32000, 31872, 31744, 31616, 31488, 31360, 31232, 31104, 30976, 30848, 30720,
  30592, 30464, 30336, 30208, 30080, 29952, 29824, 29696, 29568, 29440, 29312, 29184, 29056, 28928, 28800, 28672,
  28544, 28416, 28288, 28160, 28032, 27904, 27776, 27648, 27520, 27392, 27264, 27136, 27008, 26880, 26752, 26624,
  26496, 26368, 26240, 26112, 25984, 25856, 25728, 25600, 25472, 25344, 25216, 25088, 24960, 24832, 24704, 24576,
  24448, 24320, 24192, 24064, 23936, 23808, 23680, 23552, 23424, 23296, 23168, 23040, 22912, 22784, 22656, 22528,
  22400, 22272, 22144, 22016, 21888, 21760, 21632, 21504, 21376, 21248, 21120, 20992, 20864, 20736, 20608, 20480,
  20352, 20224, 20096, 19968, 19840, 19712, 19584, 19456, 19328, 19200, 19072, 18944, 18816, 18688, 18560, 18432,
  18304, 18176, 18048, 17920, 17792, 17664, 17536, 17408, 17280, 17152, 17024, 16896, 16768, 16640, 16512, 16384,
  16256, 16128, 16000, 15872, 15744, 15616, 15488, 15360, 15232, 15104, 14976, 14848, 14720, 14592, 14464, 14336,
  14208, 14080, 13952, 13824, 13696, 13568, 13440, 13312, 13184, 13056, 12928, 12800, 12672, 12544, 12416, 12288,
  12160, 12032, 11904, 11776, 11648, 11520, 11392, 11264, 11136, 11008, 10880, 10752, 10624, 10496, 10368, 10240,
  10112, 9984, 9856, 9728, 9600, 9472, 9344, 9216, 9088, 8960, 8832, 8704, 8576, 8448, 8320, 8192,
  8064, 7936, 7808, 7680, 7552, 7424, 7296, 7168, 7040, 6912, 6784, 6656, 6528, 6400, 6272, 6144,
  6016, 5888, 5760, 5632, 5504, 5376, 5248, 5120, 4992, 4864, 4736, 4608, 4480, 4352, 4224, 4096,
  3968, 3840, 3712, 3584, 3456, 3328, 3200, 3072, 2944, 2816, 2688, 2560, 2432, 2304, 2176, 2048,
  1920, 1792, 1664, 1536, 1408, 1280, 1152, 1024, 896, 768, 640, 512, 384, 256, 128, 0,
  -128, -256, -384, -512, -640, -768, -896, -1024, -1152, -1280, -1408, -1536, -1664, -1792, -1920, -2048,
  -2176, -2304, -2432, -2560, -2688, -2816, -2944, -3072, -3200, -3328, -3456, -3584, -3712, -3840, -3968, -4096,
  -4224, -4352, -4480, -4608, -4736, -4864, -4992, -5120, -5248, -5376, -5504, -5632, -5760, -5888, -6016, -6144,
  -6272, -6400, -6528, -6656, -6784, -6912, -7040, -7168, -7296, -7424, -7552, -7680, -7808, -7936, -8064, -8192,
  -8320, -8448, -8576, -8704, -8832, -8960, -9088, -9216, -9344, -9472, -9600, -9728, -9856, -9984, -10112, -10240,
  -10368, -10496, -10624, -10752, -10880, -11008, -11136, -11264, -11392, -11520, -11648, -11776, -11904, -12032, -12160, -12288,
  -12416, -12544, -12672, -12800, -12928, -13056, -13184, -13312, -13440, -13568, -13696, -13824, -13952, -14080, -14208, -14336,
  -14464, -14592, -14720, -14848, -14976, -15104, -15232, -15360, -15488, -15616, -15744, -15872, -16000, -16128, -16256, -16384,
  -16512, -16640, -16768, -16896, -17024, -17152, -17280, -17408, -17536, -17664, -17792, -17920, -18048, -18176, -18304, -18432,
  -18560, -18688, -18816, -18944, -19072, -19200, -19328, -19456, -19584, -19712, -19840, -19968, -20096, -20224, -20352, -20480,
  -20608, -20736, -20864, -20992, -21120, -21248, -21376, -21504, -21632, -21760, -21888, -22016, -22144, -22272, -22400, -22528,
  -22656, -22784, -22912, -23040, -23168, -23296, -23424, -23552, -23680, -23808, -23936, -24064, -24192, -24320, -24448, -24576,
  -24704, -24832, -24960, -25088, -25216, -25344, -25472, -25600, -25728, -25856, -25984, -26112, -26240, -26368, -26496, -26624,
  -26752, -26880, -27008, -27136, -27264, -27392, -27520, -27648, -27776, -27904, -28032, -28160, -28288, -28416, -28544, -28672,
  -28800, -28928, -29056, -29184, -29312, -29440, -29568, -29696, -29824, -29952, -30080, -30208, -30336, -30464, -30592, -30720,
  -30848, -30976, -31104, -31232, -31360, -31488, -31616, -31744, -31872, -32000, -32128, -32256, -32384, -32512, -32640, 0,
};

const int16_t distoTable[] = {
  774,
  3255, 5130, 5823, 5674, 5738, 6628, 7769, 8453, 8125, 6772, 5433, 5936, 8441, 11545, 14187, 16225,
  17799, 19114, 20245, 21268, 22183, 23027, 23800, 24515, 25173, 25777, 26329, 26829, 27286, 27697, 28070, 28406,
  28714, 28990, 29245, 29472, 29686, 29877, 30056, 30218, 30371, 30510, 30640, 30759, 30871, 30975, 31070, 31162,
  31246, 31326, 31401, 31471, 31536, 31599, 31657, 31711, 31764, 31812, 31858, 31902, 31944, 31985, 32021, 32057,
  32092, 32123, 32155, 32184, 32211, 32238, 32263, 32287, 32311, 32333, 32354, 32376, 32394, 32412, 32432, 32448,
  32464, 32480, 32495, 32508, 32524, 32536, 32549, 32561, 32573, 32583, 32595, 32604, 32615, 32624, 32633, 32641,
  32648, 32659, 32665, 32672, 32679, 32685, 32691, 32697, 32703, 32708, 32714, 32719, 32723, 32728, 32731, 32734,
  32739, 32743, 32746, 32748, 32750, 32753, 32755, 32758, 32760, 32761, 32763, 32765, 32764, 32765, 32766, 32766,
  32766, 32766, 32766, 32766, 32765, 32765, 32764, 32763, 32762, 32759, 32757, 32754, 32752, 32750, 32747, 32744,
  32742, 32738, 32734, 32730, 32726, 32722, 32717, 32712, 32708, 32702, 32697, 32691, 32683, 32678, 32670, 32664,
  32656, 32648, 32641, 32631, 32622, 32613, 32604, 32593, 32582, 32571, 32560, 32549, 32537, 32522, 32510, 32495,
  32481, 32465, 32448, 32434, 32415, 32398, 32379, 32359, 32338, 32316, 32294, 32269, 32244, 32219, 32192, 32164,
  32133, 32103, 32070, 32035, 31999, 31960, 31920, 31878, 31834, 31786, 31737, 31684, 31627, 31569, 31506, 31438,
  31367, 31291, 31210, 31124, 31032, 30932, 30825, 30711, 30586, 30453, 30309, 30151, 29978, 29791, 29587, 29362,
  29115, 28842, 28539, 28201, 27825, 27401, 26923, 26382, 25765, 25057, 24240, 23286, 22073, 20404, 18362, 16720,
  16216, 16589, 17105, 17205, 16646, 15618, 14781, 14472, 14157, 13269, 11908, 10288, 8022, 5041, 2044, -673,
  -3179, -5047, -5747, -5585, -5663, -6546, -7702, -8376, -8056, -6688, -5353, -5842, -8368, -11477, -14139, -16178,
  -17763, -19077, -20220, -21239, -22162, -23006, -23782, -24497, -25158, -25765, -26316, -26820, -27275, -27689, -28062, -28399,
  -28706, -28986, -29239, -29470, -29680, -29875, -30051, -30215, -30367, -30506, -30637, -30756, -30868, -30973, -31069, -31161,
  -31245, -31324, -31399, -31469, -31534, -31597, -31655, -31709, -31762, -31811, -31858, -31902, -31944, -31983, -32021, -32057,
  -32090, -32123, -32153, -32183, -32211, -32238, -32263, -32286, -32310, -32334, -32354, -32375, -32394, -32412, -32430, -32448,
  -32463, -32480, -32494, -32509, -32524, -32535, -32548, -32560, -32573, -32584, -32593, -32604, -32613, -32623, -32632, -32640,
  -32649, -32657, -32664, -32672, -32679, -32685, -32691, -32697, -32703, -32708, -32714, -32719, -32723, -32728, -32731, -32735,
  -32738, -32742, -32745, -32748, -32751, -32753, -32755, -32758, -32758, -32761, -32763, -32763, -32765, -32764, -32766, -32766,
  -32766, -32766, -32766, -32766, -32765, -32764, -32763, -32762, -32760, -32759, -32757, -32754, -32752, -32750, -32747, -32744,
  -32741, -32738, -32734, -32730, -32726, -32722, -32716, -32712, -32707, -32702, -32696, -32690, -32684, -32677, -32671, -32663,
  -32655, -32647, -32640, -32631, -32622, -32613, -32603, -32593, -32582, -32571, -32560, -32548, -32535, -32523, -32509, -32495,
  -32480, -32464, -32449, -32432, -32415, -32397, -32377, -32357, -32337, -32314, -32292, -32269, -32243, -32218, -32193, -32163,
  -32132, -32102, -32069, -32034, -31998, -31961, -31919, -31878, -31833, -31785, -31735, -31682, -31627, -31567, -31504, -31438,
  -31366, -31289, -31208, -31122, -31031, -30930, -30823, -30709, -30584, -30449, -30306, -30146, -29977, -29787, -29584, -29357,
  -29111, -28835, -28533, -28194, -27817, -27390, -26914, -26370, -25752, -25040, -24224, -23265, -22051, -20374, -18324, -16679,
  -16171, -16550, -17059, -17167, -16600, -15582, -14726, -14432, -14099, -13227, -11843, -10241, -7938, -4989, -1915,
};

int numWaves = 4;
const int16_t *wavePointer[] = {sinTable, triTable, sawTable , distoTable};
const int16_t *waveLink;

uint8_t timerPin[] = {4,5,25};
uint8_t out2pin[] = {23, 0, 22, 25, 20, 6, 21, 5, 9, 4, 10, 3};
uint8_t maxPower = 0;
uint8_t lastPower = 0;
int notes = 0;

//oscillators
uint32_t Ophase_inc[3];
uint32_t Ophase[3];
uint32_t Ophase2[3];
uint8_t Opower[3];
uint8_t Ooctave[3];
uint8_t Oactive[3];
int32_t Owave[3];
int32_t Owave2[3];
uint32_t note2inc[128];
int32_t Ooutput;
int32_t FMindex;

//uint8_t cc2out[] = {40, 41, 42, 1, 44, 45, 46, 47, 48, 49, 50, 51};

void ISR_OSC(void);
void ISR_OSC1(void);
void ISR_OSC2(void);

//IntervalTimer OSC;

void setup() {
  // Initiate MIDI communications, listen to all channels

  for (int i = 0; i < 12; i ++) {
    if (out2pin[i]) {
      pinMode(out2pin[i], OUTPUT);
      analogWriteFrequency(out2pin[i], 46875);
    }
  }

  analogWriteResolution(10);

  //populate the exponential lookup table (ok for MIDI since there are only 128 notes)
  for (int i; i < 128; i++) {
    note2inc[i] = uint32_t(1046.56 * pow(2.0, i / 12.0)); //not exact 440 tuning for now
  }

  attachInterrupt(timerPin[0],ISR_OSC, RISING);
  attachInterrupt(timerPin[1],ISR_OSC1, RISING);
  attachInterrupt(timerPin[2],ISR_OSC2, RISING);


  //digitalWriteFast(4, HIGH);
  //----------------------light animation--------------------
  for (int i = 0; i < 12; i ++) {
    for (int j = 0; j < 128; j ++) {
      if (out2pin[i] == 0) analogWrite(A14, j << 3);
      else analogWrite(out2pin[i], j << 3 );
      delay(1);
    }
    if (out2pin[i] == 0) analogWrite(A14, 0);
    analogWrite(out2pin[i], 0);
    //------------------end light animation---------------------
  }

  MIDI.begin(MIDI_CHANNEL_OMNI);
  // Connect the function to the library, so it is called upon reception of a NoteOn.
  MIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
  MIDI.setHandleNoteOff(HandleNoteOff);
  MIDI.setHandleControlChange(HandleControlChange);
  MIDI.setHandlePitchBend (HandlePitchBend);
analogWrite(timerPin[0],512);
analogWrite(timerPin[1],512);
analogWrite(timerPin[2],512);
  Serial.begin(9600);
}

void loop() {
  // Call MIDI.read the fastest you can for real-time performance.
  MIDI.read();
  // There is no need to check if there are messages incoming if they are bound to a Callback function.
}

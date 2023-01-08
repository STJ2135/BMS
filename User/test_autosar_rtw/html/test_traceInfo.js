function RTW_Sid2UrlHash() {
	this.urlHashMap = new Array();
	/* <Root>/current */
	this.urlHashMap["test:1"] = "test.c:92,159,186,321,339,378";
	/* <Root>/voltage_V1 */
	this.urlHashMap["test:2"] = "test.c:112,251";
	/* <Root>/voltage_V2 */
	this.urlHashMap["test:3"] = "test.c:113,252";
	/* <Root>/voltage_V3 */
	this.urlHashMap["test:4"] = "test.c:114,253";
	/* <Root>/voltage_V4 */
	this.urlHashMap["test:5"] = "test.c:115,254";
	/* <Root>/voltage_V5 */
	this.urlHashMap["test:6"] = "test.c:116,255";
	/* <Root>/BMS_CmdOpenBalan */
	this.urlHashMap["test:7"] = "test.c:284,294";
	/* <Root>/BMS_CmdOpenCHG */
	this.urlHashMap["test:8"] = "test.c:274,313";
	/* <Root>/BMS_CmdOpenDSG */
	this.urlHashMap["test:9"] = "test.c:304,330";
	/* <Root>/BMS_Cmd */
	this.urlHashMap["test:10"] = "test.c:267";
	/* <Root>/Abs */
	this.urlHashMap["test:11"] = "test.c:156";
	/* <Root>/Constant */
	this.urlHashMap["test:25"] = "test.c:90";
	/* <Root>/Constant1 */
	this.urlHashMap["test:26"] = "test.c:91";
	/* <Root>/Constant2 */
	this.urlHashMap["test:27"] = "test.c:99";
	/* <Root>/Gain */
	this.urlHashMap["test:29"] = "test.c:182";
	/* <Root>/Gain1 */
	this.urlHashMap["test:30"] = "test.c:235";
	/* <Root>/Lookup
Table */
	this.urlHashMap["test:49"] = "test.c:146&test.h:45,50&test_data.c:23,28";
	/* <Root>/Saturation */
	this.urlHashMap["test:50"] = "test.c:217,226";
	/* <Root>/Saturation1 */
	this.urlHashMap["test:51"] = "test.c:205,214";
	/* <Root>/Switch */
	this.urlHashMap["test:113"] = "test.c:89,106";
	/* <Root>/Switch1 */
	this.urlHashMap["test:114"] = "test.c:93,98";
	/* <Root>/Batt_State */
	this.urlHashMap["test:134"] = "test.c:108";
	/* <Root>/SOC */
	this.urlHashMap["test:135"] = "test.c:234";
	/* <Root>/OutCurrent */
	this.urlHashMap["test:136"] = "test.c:377";
	/* <Root>/OutBus_Voltage */
	this.urlHashMap["test:137"] = "test.c:250";
	/* <Root>/OutCmdCloseCHG */
	this.urlHashMap["test:138"] = "test.c:271";
	/* <Root>/OutCmdOpenCHG */
	this.urlHashMap["test:139"] = "test.c:372";
	/* <Root>/OutCmdOpenBalan */
	this.urlHashMap["test:140"] = "test.c:281";
	/* <Root>/OutCmdCloseBalan */
	this.urlHashMap["test:141"] = "test.c:291";
	/* <Root>/OutCmdOpenDSG */
	this.urlHashMap["test:142"] = "test.c:367";
	/* <Root>/OutCmdCloseDSG */
	this.urlHashMap["test:143"] = "test.c:301";
	/* <Root>/Batt_Fault */
	this.urlHashMap["test:144"] = "test.c:364";
	/* <S1>/Add */
	this.urlHashMap["test:16"] = "test.c:189";
	/* <S1>/Add1 */
	this.urlHashMap["test:17"] = "test.c:190";
	/* <S1>/Delay */
	this.urlHashMap["test:18"] = "test.c:150,181,389,414&test.h:33,38";
	/* <S1>/Gain1 */
	this.urlHashMap["test:19"] = "test.c:183";
	/* <S1>/Gain2 */
	this.urlHashMap["test:20"] = "test.c:184";
	/* <S1>/Gain3 */
	this.urlHashMap["test:21"] = "test.c:185";
	/* <S1>/Switch */
	this.urlHashMap["test:22"] = "test.c:179,201";
	/* <S2>/Compare */
	this.urlHashMap["test:24:2"] = "test.c:265";
	/* <S2>/Constant */
	this.urlHashMap["test:24:3"] = "test.c:266";
	/* <S3>/Add */
	this.urlHashMap["test:36"] = "test.c:228";
	/* <S3>/Delay */
	this.urlHashMap["test:37"] = "test.c:203,229,239,401,417&test.h:35,39";
	/* <S3>/Gain1 */
	this.urlHashMap["test:38"] = "test.c:230";
	/* <S3>/Gain2 */
	this.urlHashMap["test:39"] = "test.c:240";
	/* <S4>/Add */
	this.urlHashMap["test:43"] = "test.c:129";
	/* <S4>/Delay */
	this.urlHashMap["test:44"] = "test.c:124,130,383,411&test.h:32,37";
	/* <S4>/Gain1 */
	this.urlHashMap["test:45"] = "test.c:131";
	/* <S4>/Gain2 */
	this.urlHashMap["test:46"] = "test.c:384";
	/* <S4>/Saturation */
	this.urlHashMap["test:47"] = "test.c:135,144";
	/* <S5>/Add */
	this.urlHashMap["test:58"] = "test.c:256";
	/* <S5>/MinMax */
	this.urlHashMap["test:59"] = "test.c:111";
	/* <S6>/Data Type Conversion1 */
	this.urlHashMap["test:66"] = "test.c:283";
	/* <S6>/Data Type Conversion2 */
	this.urlHashMap["test:67"] = "test.c:293";
	/* <S6>/Logical
Operator */
	this.urlHashMap["test:70"] = "test.c:295";
	/* <S6>/Logical
Operator1 */
	this.urlHashMap["test:71"] = "test.c:285";
	/* <S7>/Data Type Conversion3 */
	this.urlHashMap["test:78"] = "test.c:373";
	/* <S7>/Data Type Conversion4 */
	this.urlHashMap["test:79"] = "test.c:273";
	/* <S7>/Logical
Operator */
	this.urlHashMap["test:82"] = "test.c:275";
	/* <S7>/Logical
Operator1 */
	this.urlHashMap["test:83"] = "test.c:311";
	/* <S8>/Data Type Conversion5 */
	this.urlHashMap["test:90"] = "test.c:368";
	/* <S8>/Data Type Conversion6 */
	this.urlHashMap["test:91"] = "test.c:303";
	/* <S8>/Logical
Operator */
	this.urlHashMap["test:94"] = "test.c:305";
	/* <S8>/Logical
Operator1 */
	this.urlHashMap["test:95"] = "test.c:328";
	/* <S9>/Constant */
	this.urlHashMap["test:102"] = "test.c:347";
	/* <S9>/Constant1 */
	this.urlHashMap["test:103"] = "test.c:348";
	/* <S9>/Data Type Conversion1 */
	this.urlHashMap["test:104"] = "msg=rtwMsg_CodeGenerationReducedBlock&block=test:104";
	/* <S9>/Data Type Conversion7 */
	this.urlHashMap["test:105"] = "test.c:319";
	/* <S9>/Logical
Operator */
	this.urlHashMap["test:108"] = "test.c:322,340";
	/* <S9>/Logical
Operator1 */
	this.urlHashMap["test:109"] = "test.c:349";
	/* <S9>/Switch */
	this.urlHashMap["test:110"] = "test.c:337,362";
	/* <S9>/Switch1 */
	this.urlHashMap["test:111"] = "test.c:345,359";
	/* <S10>/Add */
	this.urlHashMap["test:117"] = "test.c:175,397";
	/* <S10>/Constant */
	this.urlHashMap["test:119"] = "test.c:157,171,180";
	/* <S10>/Constant1 */
	this.urlHashMap["test:120"] = "test.c:172,394";
	/* <S10>/Data Type Conversion */
	this.urlHashMap["test:121"] = "msg=rtwMsg_CodeGenerationReducedBlock&block=test:121";
	/* <S10>/Data Type Conversion1 */
	this.urlHashMap["test:122"] = "msg=rtwMsg_CodeGenerationReducedBlock&block=test:122";
	/* <S10>/Data Type Conversion2 */
	this.urlHashMap["test:123"] = "test.c:247";
	/* <S10>/Delay */
	this.urlHashMap["test:124"] = "test.c:173,393&test.h:34";
	/* <S10>/Delay1 */
	this.urlHashMap["test:125"] = "test.c:395,404&test.h:36";
	/* <S10>/Gain */
	this.urlHashMap["test:126"] = "test.c:174";
	/* <S10>/Logical
Operator */
	this.urlHashMap["test:127"] = "test.c:187";
	/* <S10>/Logical
Operator1 */
	this.urlHashMap["test:128"] = "test.c:244";
	/* <S10>/Product */
	this.urlHashMap["test:129"] = "test.c:396";
	/* <S10>/Relational
Operator */
	this.urlHashMap["test:130"] = "test.c:188";
	/* <S10>/Relational
Operator1 */
	this.urlHashMap["test:131"] = "test.c:170";
	/* <S10>/count */
	this.urlHashMap["test:132"] = "test.c:155,168&test.h:31";
	/* <S11>/Compare */
	this.urlHashMap["test:64:2"] = "test.c:286";
	/* <S11>/Constant */
	this.urlHashMap["test:64:3"] = "test.c:282";
	/* <S12>/Compare */
	this.urlHashMap["test:65:2"] = "test.c:296";
	/* <S12>/Constant */
	this.urlHashMap["test:65:3"] = "test.c:292";
	/* <S13>/Compare */
	this.urlHashMap["test:76:2"] = "test.c:314";
	/* <S13>/Constant */
	this.urlHashMap["test:76:3"] = "test.c:312";
	/* <S14>/Compare */
	this.urlHashMap["test:77:2"] = "test.c:276";
	/* <S14>/Constant */
	this.urlHashMap["test:77:3"] = "test.c:272";
	/* <S15>/Compare */
	this.urlHashMap["test:88:2"] = "test.c:331";
	/* <S15>/Constant */
	this.urlHashMap["test:88:3"] = "test.c:329";
	/* <S16>/Compare */
	this.urlHashMap["test:89:2"] = "test.c:306";
	/* <S16>/Constant */
	this.urlHashMap["test:89:3"] = "test.c:302";
	/* <S17>/Compare */
	this.urlHashMap["test:100:2"] = "test.c:350";
	/* <S17>/Constant */
	this.urlHashMap["test:100:3"] = "test.c:346";
	/* <S18>/Compare */
	this.urlHashMap["test:101:2"] = "test.c:323,341";
	/* <S18>/Constant */
	this.urlHashMap["test:101:3"] = "test.c:320,338";
	/* <S19>/Compare */
	this.urlHashMap["test:118:2"] = "test.c:160";
	/* <S19>/Constant */
	this.urlHashMap["test:118:3"] = "test.c:158";
	/* <S20>:54 */
	this.urlHashMap["test:132:54"] = "msg=rtwMsg_optimizedSfObject&block=test:132:54";
	/* <S20>:53 */
	this.urlHashMap["test:132:53"] = "msg=rtwMsg_optimizedSfObject&block=test:132:53";
	/* <S20>:16 */
	this.urlHashMap["test:132:16"] = "msg=rtwMsg_optimizedSfObject&block=test:132:16";
	/* <S20>:56 */
	this.urlHashMap["test:132:56"] = "msg=rtwMsg_optimizedSfObject&block=test:132:56";
	/* <S20>:28 */
	this.urlHashMap["test:132:28"] = "msg=rtwMsg_optimizedSfObject&block=test:132:28";
	/* <S20>:41 */
	this.urlHashMap["test:132:41"] = "msg=rtwMsg_optimizedSfObject&block=test:132:41";
	/* <S20>:57 */
	this.urlHashMap["test:132:57"] = "msg=rtwMsg_optimizedSfObject&block=test:132:57";
	/* <S20>:49 */
	this.urlHashMap["test:132:49"] = "msg=rtwMsg_optimizedSfObject&block=test:132:49";
	/* <S20>:27 */
	this.urlHashMap["test:132:27"] = "msg=rtwMsg_optimizedSfObject&block=test:132:27";
	this.getUrlHash = function(sid) { return this.urlHashMap[sid];}
}
RTW_Sid2UrlHash.instance = new RTW_Sid2UrlHash();
function RTW_rtwnameSIDMap() {
	this.rtwnameHashMap = new Array();
	this.sidHashMap = new Array();
	this.rtwnameHashMap["<Root>"] = {sid: "test"};
	this.sidHashMap["test"] = {rtwname: "<Root>"};
	this.rtwnameHashMap["<S1>"] = {sid: "test:12"};
	this.sidHashMap["test:12"] = {rtwname: "<S1>"};
	this.rtwnameHashMap["<S2>"] = {sid: "test:24"};
	this.sidHashMap["test:24"] = {rtwname: "<S2>"};
	this.rtwnameHashMap["<S3>"] = {sid: "test:33"};
	this.sidHashMap["test:33"] = {rtwname: "<S3>"};
	this.rtwnameHashMap["<S4>"] = {sid: "test:41"};
	this.sidHashMap["test:41"] = {rtwname: "<S4>"};
	this.rtwnameHashMap["<S5>"] = {sid: "test:52"};
	this.sidHashMap["test:52"] = {rtwname: "<S5>"};
	this.rtwnameHashMap["<S6>"] = {sid: "test:62"};
	this.sidHashMap["test:62"] = {rtwname: "<S6>"};
	this.rtwnameHashMap["<S7>"] = {sid: "test:74"};
	this.sidHashMap["test:74"] = {rtwname: "<S7>"};
	this.rtwnameHashMap["<S8>"] = {sid: "test:86"};
	this.sidHashMap["test:86"] = {rtwname: "<S8>"};
	this.rtwnameHashMap["<S9>"] = {sid: "test:98"};
	this.sidHashMap["test:98"] = {rtwname: "<S9>"};
	this.rtwnameHashMap["<S10>"] = {sid: "test:115"};
	this.sidHashMap["test:115"] = {rtwname: "<S10>"};
	this.rtwnameHashMap["<S11>"] = {sid: "test:64"};
	this.sidHashMap["test:64"] = {rtwname: "<S11>"};
	this.rtwnameHashMap["<S12>"] = {sid: "test:65"};
	this.sidHashMap["test:65"] = {rtwname: "<S12>"};
	this.rtwnameHashMap["<S13>"] = {sid: "test:76"};
	this.sidHashMap["test:76"] = {rtwname: "<S13>"};
	this.rtwnameHashMap["<S14>"] = {sid: "test:77"};
	this.sidHashMap["test:77"] = {rtwname: "<S14>"};
	this.rtwnameHashMap["<S15>"] = {sid: "test:88"};
	this.sidHashMap["test:88"] = {rtwname: "<S15>"};
	this.rtwnameHashMap["<S16>"] = {sid: "test:89"};
	this.sidHashMap["test:89"] = {rtwname: "<S16>"};
	this.rtwnameHashMap["<S17>"] = {sid: "test:100"};
	this.sidHashMap["test:100"] = {rtwname: "<S17>"};
	this.rtwnameHashMap["<S18>"] = {sid: "test:101"};
	this.sidHashMap["test:101"] = {rtwname: "<S18>"};
	this.rtwnameHashMap["<S19>"] = {sid: "test:118"};
	this.sidHashMap["test:118"] = {rtwname: "<S19>"};
	this.rtwnameHashMap["<S20>"] = {sid: "test:132"};
	this.sidHashMap["test:132"] = {rtwname: "<S20>"};
	this.rtwnameHashMap["<Root>/current"] = {sid: "test:1"};
	this.sidHashMap["test:1"] = {rtwname: "<Root>/current"};
	this.rtwnameHashMap["<Root>/voltage_V1"] = {sid: "test:2"};
	this.sidHashMap["test:2"] = {rtwname: "<Root>/voltage_V1"};
	this.rtwnameHashMap["<Root>/voltage_V2"] = {sid: "test:3"};
	this.sidHashMap["test:3"] = {rtwname: "<Root>/voltage_V2"};
	this.rtwnameHashMap["<Root>/voltage_V3"] = {sid: "test:4"};
	this.sidHashMap["test:4"] = {rtwname: "<Root>/voltage_V3"};
	this.rtwnameHashMap["<Root>/voltage_V4"] = {sid: "test:5"};
	this.sidHashMap["test:5"] = {rtwname: "<Root>/voltage_V4"};
	this.rtwnameHashMap["<Root>/voltage_V5"] = {sid: "test:6"};
	this.sidHashMap["test:6"] = {rtwname: "<Root>/voltage_V5"};
	this.rtwnameHashMap["<Root>/BMS_CmdOpenBalan"] = {sid: "test:7"};
	this.sidHashMap["test:7"] = {rtwname: "<Root>/BMS_CmdOpenBalan"};
	this.rtwnameHashMap["<Root>/BMS_CmdOpenCHG"] = {sid: "test:8"};
	this.sidHashMap["test:8"] = {rtwname: "<Root>/BMS_CmdOpenCHG"};
	this.rtwnameHashMap["<Root>/BMS_CmdOpenDSG"] = {sid: "test:9"};
	this.sidHashMap["test:9"] = {rtwname: "<Root>/BMS_CmdOpenDSG"};
	this.rtwnameHashMap["<Root>/BMS_Cmd"] = {sid: "test:10"};
	this.sidHashMap["test:10"] = {rtwname: "<Root>/BMS_Cmd"};
	this.rtwnameHashMap["<Root>/Abs"] = {sid: "test:11"};
	this.sidHashMap["test:11"] = {rtwname: "<Root>/Abs"};
	this.rtwnameHashMap["<Root>/Ah_count"] = {sid: "test:12"};
	this.sidHashMap["test:12"] = {rtwname: "<Root>/Ah_count"};
	this.rtwnameHashMap["<Root>/Compare To Constant6"] = {sid: "test:24"};
	this.sidHashMap["test:24"] = {rtwname: "<Root>/Compare To Constant6"};
	this.rtwnameHashMap["<Root>/Constant"] = {sid: "test:25"};
	this.sidHashMap["test:25"] = {rtwname: "<Root>/Constant"};
	this.rtwnameHashMap["<Root>/Constant1"] = {sid: "test:26"};
	this.sidHashMap["test:26"] = {rtwname: "<Root>/Constant1"};
	this.rtwnameHashMap["<Root>/Constant2"] = {sid: "test:27"};
	this.sidHashMap["test:27"] = {rtwname: "<Root>/Constant2"};
	this.rtwnameHashMap["<Root>/From"] = {sid: "test:28"};
	this.sidHashMap["test:28"] = {rtwname: "<Root>/From"};
	this.rtwnameHashMap["<Root>/Gain"] = {sid: "test:29"};
	this.sidHashMap["test:29"] = {rtwname: "<Root>/Gain"};
	this.rtwnameHashMap["<Root>/Gain1"] = {sid: "test:30"};
	this.sidHashMap["test:30"] = {rtwname: "<Root>/Gain1"};
	this.rtwnameHashMap["<Root>/Goto"] = {sid: "test:31"};
	this.sidHashMap["test:31"] = {rtwname: "<Root>/Goto"};
	this.rtwnameHashMap["<Root>/Goto1"] = {sid: "test:32"};
	this.sidHashMap["test:32"] = {rtwname: "<Root>/Goto1"};
	this.rtwnameHashMap["<Root>/LPF"] = {sid: "test:33"};
	this.sidHashMap["test:33"] = {rtwname: "<Root>/LPF"};
	this.rtwnameHashMap["<Root>/LPF1"] = {sid: "test:41"};
	this.sidHashMap["test:41"] = {rtwname: "<Root>/LPF1"};
	this.rtwnameHashMap["<Root>/Lookup Table"] = {sid: "test:49"};
	this.sidHashMap["test:49"] = {rtwname: "<Root>/Lookup Table"};
	this.rtwnameHashMap["<Root>/Saturation"] = {sid: "test:50"};
	this.sidHashMap["test:50"] = {rtwname: "<Root>/Saturation"};
	this.rtwnameHashMap["<Root>/Saturation1"] = {sid: "test:51"};
	this.sidHashMap["test:51"] = {rtwname: "<Root>/Saturation1"};
	this.rtwnameHashMap["<Root>/Subsystem"] = {sid: "test:52"};
	this.sidHashMap["test:52"] = {rtwname: "<Root>/Subsystem"};
	this.rtwnameHashMap["<Root>/Subsystem1"] = {sid: "test:62"};
	this.sidHashMap["test:62"] = {rtwname: "<Root>/Subsystem1"};
	this.rtwnameHashMap["<Root>/Subsystem2"] = {sid: "test:74"};
	this.sidHashMap["test:74"] = {rtwname: "<Root>/Subsystem2"};
	this.rtwnameHashMap["<Root>/Subsystem3"] = {sid: "test:86"};
	this.sidHashMap["test:86"] = {rtwname: "<Root>/Subsystem3"};
	this.rtwnameHashMap["<Root>/Subsystem4"] = {sid: "test:98"};
	this.sidHashMap["test:98"] = {rtwname: "<Root>/Subsystem4"};
	this.rtwnameHashMap["<Root>/Switch"] = {sid: "test:113"};
	this.sidHashMap["test:113"] = {rtwname: "<Root>/Switch"};
	this.rtwnameHashMap["<Root>/Switch1"] = {sid: "test:114"};
	this.sidHashMap["test:114"] = {rtwname: "<Root>/Switch1"};
	this.rtwnameHashMap["<Root>/standstill"] = {sid: "test:115"};
	this.sidHashMap["test:115"] = {rtwname: "<Root>/standstill"};
	this.rtwnameHashMap["<Root>/Batt_State"] = {sid: "test:134"};
	this.sidHashMap["test:134"] = {rtwname: "<Root>/Batt_State"};
	this.rtwnameHashMap["<Root>/SOC"] = {sid: "test:135"};
	this.sidHashMap["test:135"] = {rtwname: "<Root>/SOC"};
	this.rtwnameHashMap["<Root>/OutCurrent"] = {sid: "test:136"};
	this.sidHashMap["test:136"] = {rtwname: "<Root>/OutCurrent"};
	this.rtwnameHashMap["<Root>/OutBus_Voltage"] = {sid: "test:137"};
	this.sidHashMap["test:137"] = {rtwname: "<Root>/OutBus_Voltage"};
	this.rtwnameHashMap["<Root>/OutCmdCloseCHG"] = {sid: "test:138"};
	this.sidHashMap["test:138"] = {rtwname: "<Root>/OutCmdCloseCHG"};
	this.rtwnameHashMap["<Root>/OutCmdOpenCHG"] = {sid: "test:139"};
	this.sidHashMap["test:139"] = {rtwname: "<Root>/OutCmdOpenCHG"};
	this.rtwnameHashMap["<Root>/OutCmdOpenBalan"] = {sid: "test:140"};
	this.sidHashMap["test:140"] = {rtwname: "<Root>/OutCmdOpenBalan"};
	this.rtwnameHashMap["<Root>/OutCmdCloseBalan"] = {sid: "test:141"};
	this.sidHashMap["test:141"] = {rtwname: "<Root>/OutCmdCloseBalan"};
	this.rtwnameHashMap["<Root>/OutCmdOpenDSG"] = {sid: "test:142"};
	this.sidHashMap["test:142"] = {rtwname: "<Root>/OutCmdOpenDSG"};
	this.rtwnameHashMap["<Root>/OutCmdCloseDSG"] = {sid: "test:143"};
	this.sidHashMap["test:143"] = {rtwname: "<Root>/OutCmdCloseDSG"};
	this.rtwnameHashMap["<Root>/Batt_Fault"] = {sid: "test:144"};
	this.sidHashMap["test:144"] = {rtwname: "<Root>/Batt_Fault"};
	this.rtwnameHashMap["<S1>/IL"] = {sid: "test:13"};
	this.sidHashMap["test:13"] = {rtwname: "<S1>/IL"};
	this.rtwnameHashMap["<S1>/state"] = {sid: "test:14"};
	this.sidHashMap["test:14"] = {rtwname: "<S1>/state"};
	this.rtwnameHashMap["<S1>/soc0"] = {sid: "test:15"};
	this.sidHashMap["test:15"] = {rtwname: "<S1>/soc0"};
	this.rtwnameHashMap["<S1>/Add"] = {sid: "test:16"};
	this.sidHashMap["test:16"] = {rtwname: "<S1>/Add"};
	this.rtwnameHashMap["<S1>/Add1"] = {sid: "test:17"};
	this.sidHashMap["test:17"] = {rtwname: "<S1>/Add1"};
	this.rtwnameHashMap["<S1>/Delay"] = {sid: "test:18"};
	this.sidHashMap["test:18"] = {rtwname: "<S1>/Delay"};
	this.rtwnameHashMap["<S1>/Gain1"] = {sid: "test:19"};
	this.sidHashMap["test:19"] = {rtwname: "<S1>/Gain1"};
	this.rtwnameHashMap["<S1>/Gain2"] = {sid: "test:20"};
	this.sidHashMap["test:20"] = {rtwname: "<S1>/Gain2"};
	this.rtwnameHashMap["<S1>/Gain3"] = {sid: "test:21"};
	this.sidHashMap["test:21"] = {rtwname: "<S1>/Gain3"};
	this.rtwnameHashMap["<S1>/Switch"] = {sid: "test:22"};
	this.sidHashMap["test:22"] = {rtwname: "<S1>/Switch"};
	this.rtwnameHashMap["<S1>/SOC"] = {sid: "test:23"};
	this.sidHashMap["test:23"] = {rtwname: "<S1>/SOC"};
	this.rtwnameHashMap["<S2>/u"] = {sid: "test:24:1"};
	this.sidHashMap["test:24:1"] = {rtwname: "<S2>/u"};
	this.rtwnameHashMap["<S2>/Compare"] = {sid: "test:24:2"};
	this.sidHashMap["test:24:2"] = {rtwname: "<S2>/Compare"};
	this.rtwnameHashMap["<S2>/Constant"] = {sid: "test:24:3"};
	this.sidHashMap["test:24:3"] = {rtwname: "<S2>/Constant"};
	this.rtwnameHashMap["<S2>/y"] = {sid: "test:24:4"};
	this.sidHashMap["test:24:4"] = {rtwname: "<S2>/y"};
	this.rtwnameHashMap["<S3>/soct"] = {sid: "test:34"};
	this.sidHashMap["test:34"] = {rtwname: "<S3>/soct"};
	this.rtwnameHashMap["<S3>/soc0"] = {sid: "test:35"};
	this.sidHashMap["test:35"] = {rtwname: "<S3>/soc0"};
	this.rtwnameHashMap["<S3>/Add"] = {sid: "test:36"};
	this.sidHashMap["test:36"] = {rtwname: "<S3>/Add"};
	this.rtwnameHashMap["<S3>/Delay"] = {sid: "test:37"};
	this.sidHashMap["test:37"] = {rtwname: "<S3>/Delay"};
	this.rtwnameHashMap["<S3>/Gain1"] = {sid: "test:38"};
	this.sidHashMap["test:38"] = {rtwname: "<S3>/Gain1"};
	this.rtwnameHashMap["<S3>/Gain2"] = {sid: "test:39"};
	this.sidHashMap["test:39"] = {rtwname: "<S3>/Gain2"};
	this.rtwnameHashMap["<S3>/SOC"] = {sid: "test:40"};
	this.sidHashMap["test:40"] = {rtwname: "<S3>/SOC"};
	this.rtwnameHashMap["<S4>/v0"] = {sid: "test:42"};
	this.sidHashMap["test:42"] = {rtwname: "<S4>/v0"};
	this.rtwnameHashMap["<S4>/Add"] = {sid: "test:43"};
	this.sidHashMap["test:43"] = {rtwname: "<S4>/Add"};
	this.rtwnameHashMap["<S4>/Delay"] = {sid: "test:44"};
	this.sidHashMap["test:44"] = {rtwname: "<S4>/Delay"};
	this.rtwnameHashMap["<S4>/Gain1"] = {sid: "test:45"};
	this.sidHashMap["test:45"] = {rtwname: "<S4>/Gain1"};
	this.rtwnameHashMap["<S4>/Gain2"] = {sid: "test:46"};
	this.sidHashMap["test:46"] = {rtwname: "<S4>/Gain2"};
	this.rtwnameHashMap["<S4>/Saturation"] = {sid: "test:47"};
	this.sidHashMap["test:47"] = {rtwname: "<S4>/Saturation"};
	this.rtwnameHashMap["<S4>/VL"] = {sid: "test:48"};
	this.sidHashMap["test:48"] = {rtwname: "<S4>/VL"};
	this.rtwnameHashMap["<S5>/voltage_V1"] = {sid: "test:53"};
	this.sidHashMap["test:53"] = {rtwname: "<S5>/voltage_V1"};
	this.rtwnameHashMap["<S5>/voltage_V2"] = {sid: "test:54"};
	this.sidHashMap["test:54"] = {rtwname: "<S5>/voltage_V2"};
	this.rtwnameHashMap["<S5>/voltage_V3"] = {sid: "test:55"};
	this.sidHashMap["test:55"] = {rtwname: "<S5>/voltage_V3"};
	this.rtwnameHashMap["<S5>/voltage_V4"] = {sid: "test:56"};
	this.sidHashMap["test:56"] = {rtwname: "<S5>/voltage_V4"};
	this.rtwnameHashMap["<S5>/voltage_V5"] = {sid: "test:57"};
	this.sidHashMap["test:57"] = {rtwname: "<S5>/voltage_V5"};
	this.rtwnameHashMap["<S5>/Add"] = {sid: "test:58"};
	this.sidHashMap["test:58"] = {rtwname: "<S5>/Add"};
	this.rtwnameHashMap["<S5>/MinMax"] = {sid: "test:59"};
	this.sidHashMap["test:59"] = {rtwname: "<S5>/MinMax"};
	this.rtwnameHashMap["<S5>/v0"] = {sid: "test:60"};
	this.sidHashMap["test:60"] = {rtwname: "<S5>/v0"};
	this.rtwnameHashMap["<S5>/Vsum"] = {sid: "test:61"};
	this.sidHashMap["test:61"] = {rtwname: "<S5>/Vsum"};
	this.rtwnameHashMap["<S6>/BMS_CmdOpenBalan"] = {sid: "test:63"};
	this.sidHashMap["test:63"] = {rtwname: "<S6>/BMS_CmdOpenBalan"};
	this.rtwnameHashMap["<S6>/Compare To Constant"] = {sid: "test:64"};
	this.sidHashMap["test:64"] = {rtwname: "<S6>/Compare To Constant"};
	this.rtwnameHashMap["<S6>/Compare To Constant1"] = {sid: "test:65"};
	this.sidHashMap["test:65"] = {rtwname: "<S6>/Compare To Constant1"};
	this.rtwnameHashMap["<S6>/Data Type Conversion1"] = {sid: "test:66"};
	this.sidHashMap["test:66"] = {rtwname: "<S6>/Data Type Conversion1"};
	this.rtwnameHashMap["<S6>/Data Type Conversion2"] = {sid: "test:67"};
	this.sidHashMap["test:67"] = {rtwname: "<S6>/Data Type Conversion2"};
	this.rtwnameHashMap["<S6>/From"] = {sid: "test:68"};
	this.sidHashMap["test:68"] = {rtwname: "<S6>/From"};
	this.rtwnameHashMap["<S6>/Goto"] = {sid: "test:69"};
	this.sidHashMap["test:69"] = {rtwname: "<S6>/Goto"};
	this.rtwnameHashMap["<S6>/Logical Operator"] = {sid: "test:70"};
	this.sidHashMap["test:70"] = {rtwname: "<S6>/Logical Operator"};
	this.rtwnameHashMap["<S6>/Logical Operator1"] = {sid: "test:71"};
	this.sidHashMap["test:71"] = {rtwname: "<S6>/Logical Operator1"};
	this.rtwnameHashMap["<S6>/OutCmdOpenBalan"] = {sid: "test:72"};
	this.sidHashMap["test:72"] = {rtwname: "<S6>/OutCmdOpenBalan"};
	this.rtwnameHashMap["<S6>/OutCmdCloseBalan"] = {sid: "test:73"};
	this.sidHashMap["test:73"] = {rtwname: "<S6>/OutCmdCloseBalan"};
	this.rtwnameHashMap["<S7>/BMS_CmdOpenCHG"] = {sid: "test:75"};
	this.sidHashMap["test:75"] = {rtwname: "<S7>/BMS_CmdOpenCHG"};
	this.rtwnameHashMap["<S7>/Compare To Constant2"] = {sid: "test:76"};
	this.sidHashMap["test:76"] = {rtwname: "<S7>/Compare To Constant2"};
	this.rtwnameHashMap["<S7>/Compare To Constant3"] = {sid: "test:77"};
	this.sidHashMap["test:77"] = {rtwname: "<S7>/Compare To Constant3"};
	this.rtwnameHashMap["<S7>/Data Type Conversion3"] = {sid: "test:78"};
	this.sidHashMap["test:78"] = {rtwname: "<S7>/Data Type Conversion3"};
	this.rtwnameHashMap["<S7>/Data Type Conversion4"] = {sid: "test:79"};
	this.sidHashMap["test:79"] = {rtwname: "<S7>/Data Type Conversion4"};
	this.rtwnameHashMap["<S7>/From"] = {sid: "test:80"};
	this.sidHashMap["test:80"] = {rtwname: "<S7>/From"};
	this.rtwnameHashMap["<S7>/Goto"] = {sid: "test:81"};
	this.sidHashMap["test:81"] = {rtwname: "<S7>/Goto"};
	this.rtwnameHashMap["<S7>/Logical Operator"] = {sid: "test:82"};
	this.sidHashMap["test:82"] = {rtwname: "<S7>/Logical Operator"};
	this.rtwnameHashMap["<S7>/Logical Operator1"] = {sid: "test:83"};
	this.sidHashMap["test:83"] = {rtwname: "<S7>/Logical Operator1"};
	this.rtwnameHashMap["<S7>/OutCmdCloseCHG"] = {sid: "test:84"};
	this.sidHashMap["test:84"] = {rtwname: "<S7>/OutCmdCloseCHG"};
	this.rtwnameHashMap["<S7>/OutCmdOpenCHG"] = {sid: "test:85"};
	this.sidHashMap["test:85"] = {rtwname: "<S7>/OutCmdOpenCHG"};
	this.rtwnameHashMap["<S8>/BMS_CmdOpenDSG"] = {sid: "test:87"};
	this.sidHashMap["test:87"] = {rtwname: "<S8>/BMS_CmdOpenDSG"};
	this.rtwnameHashMap["<S8>/Compare To Constant4"] = {sid: "test:88"};
	this.sidHashMap["test:88"] = {rtwname: "<S8>/Compare To Constant4"};
	this.rtwnameHashMap["<S8>/Compare To Constant5"] = {sid: "test:89"};
	this.sidHashMap["test:89"] = {rtwname: "<S8>/Compare To Constant5"};
	this.rtwnameHashMap["<S8>/Data Type Conversion5"] = {sid: "test:90"};
	this.sidHashMap["test:90"] = {rtwname: "<S8>/Data Type Conversion5"};
	this.rtwnameHashMap["<S8>/Data Type Conversion6"] = {sid: "test:91"};
	this.sidHashMap["test:91"] = {rtwname: "<S8>/Data Type Conversion6"};
	this.rtwnameHashMap["<S8>/From"] = {sid: "test:92"};
	this.sidHashMap["test:92"] = {rtwname: "<S8>/From"};
	this.rtwnameHashMap["<S8>/Goto"] = {sid: "test:93"};
	this.sidHashMap["test:93"] = {rtwname: "<S8>/Goto"};
	this.rtwnameHashMap["<S8>/Logical Operator"] = {sid: "test:94"};
	this.sidHashMap["test:94"] = {rtwname: "<S8>/Logical Operator"};
	this.rtwnameHashMap["<S8>/Logical Operator1"] = {sid: "test:95"};
	this.sidHashMap["test:95"] = {rtwname: "<S8>/Logical Operator1"};
	this.rtwnameHashMap["<S8>/OutCmdOpenDSG"] = {sid: "test:96"};
	this.sidHashMap["test:96"] = {rtwname: "<S8>/OutCmdOpenDSG"};
	this.rtwnameHashMap["<S8>/OutCmdCloseDSG"] = {sid: "test:97"};
	this.sidHashMap["test:97"] = {rtwname: "<S8>/OutCmdCloseDSG"};
	this.rtwnameHashMap["<S9>/Current"] = {sid: "test:99"};
	this.sidHashMap["test:99"] = {rtwname: "<S9>/Current"};
	this.rtwnameHashMap["<S9>/Compare To Constant"] = {sid: "test:100"};
	this.sidHashMap["test:100"] = {rtwname: "<S9>/Compare To Constant"};
	this.rtwnameHashMap["<S9>/Compare To Constant1"] = {sid: "test:101"};
	this.sidHashMap["test:101"] = {rtwname: "<S9>/Compare To Constant1"};
	this.rtwnameHashMap["<S9>/Constant"] = {sid: "test:102"};
	this.sidHashMap["test:102"] = {rtwname: "<S9>/Constant"};
	this.rtwnameHashMap["<S9>/Constant1"] = {sid: "test:103"};
	this.sidHashMap["test:103"] = {rtwname: "<S9>/Constant1"};
	this.rtwnameHashMap["<S9>/Data Type Conversion1"] = {sid: "test:104"};
	this.sidHashMap["test:104"] = {rtwname: "<S9>/Data Type Conversion1"};
	this.rtwnameHashMap["<S9>/Data Type Conversion7"] = {sid: "test:105"};
	this.sidHashMap["test:105"] = {rtwname: "<S9>/Data Type Conversion7"};
	this.rtwnameHashMap["<S9>/From"] = {sid: "test:106"};
	this.sidHashMap["test:106"] = {rtwname: "<S9>/From"};
	this.rtwnameHashMap["<S9>/From1"] = {sid: "test:107"};
	this.sidHashMap["test:107"] = {rtwname: "<S9>/From1"};
	this.rtwnameHashMap["<S9>/Logical Operator"] = {sid: "test:108"};
	this.sidHashMap["test:108"] = {rtwname: "<S9>/Logical Operator"};
	this.rtwnameHashMap["<S9>/Logical Operator1"] = {sid: "test:109"};
	this.sidHashMap["test:109"] = {rtwname: "<S9>/Logical Operator1"};
	this.rtwnameHashMap["<S9>/Switch"] = {sid: "test:110"};
	this.sidHashMap["test:110"] = {rtwname: "<S9>/Switch"};
	this.rtwnameHashMap["<S9>/Switch1"] = {sid: "test:111"};
	this.sidHashMap["test:111"] = {rtwname: "<S9>/Switch1"};
	this.rtwnameHashMap["<S9>/Batt_Fault"] = {sid: "test:112"};
	this.sidHashMap["test:112"] = {rtwname: "<S9>/Batt_Fault"};
	this.rtwnameHashMap["<S10>/In1"] = {sid: "test:116"};
	this.sidHashMap["test:116"] = {rtwname: "<S10>/In1"};
	this.rtwnameHashMap["<S10>/Add"] = {sid: "test:117"};
	this.sidHashMap["test:117"] = {rtwname: "<S10>/Add"};
	this.rtwnameHashMap["<S10>/Compare To Constant"] = {sid: "test:118"};
	this.sidHashMap["test:118"] = {rtwname: "<S10>/Compare To Constant"};
	this.rtwnameHashMap["<S10>/Constant"] = {sid: "test:119"};
	this.sidHashMap["test:119"] = {rtwname: "<S10>/Constant"};
	this.rtwnameHashMap["<S10>/Constant1"] = {sid: "test:120"};
	this.sidHashMap["test:120"] = {rtwname: "<S10>/Constant1"};
	this.rtwnameHashMap["<S10>/Data Type Conversion"] = {sid: "test:121"};
	this.sidHashMap["test:121"] = {rtwname: "<S10>/Data Type Conversion"};
	this.rtwnameHashMap["<S10>/Data Type Conversion1"] = {sid: "test:122"};
	this.sidHashMap["test:122"] = {rtwname: "<S10>/Data Type Conversion1"};
	this.rtwnameHashMap["<S10>/Data Type Conversion2"] = {sid: "test:123"};
	this.sidHashMap["test:123"] = {rtwname: "<S10>/Data Type Conversion2"};
	this.rtwnameHashMap["<S10>/Delay"] = {sid: "test:124"};
	this.sidHashMap["test:124"] = {rtwname: "<S10>/Delay"};
	this.rtwnameHashMap["<S10>/Delay1"] = {sid: "test:125"};
	this.sidHashMap["test:125"] = {rtwname: "<S10>/Delay1"};
	this.rtwnameHashMap["<S10>/Gain"] = {sid: "test:126"};
	this.sidHashMap["test:126"] = {rtwname: "<S10>/Gain"};
	this.rtwnameHashMap["<S10>/Logical Operator"] = {sid: "test:127"};
	this.sidHashMap["test:127"] = {rtwname: "<S10>/Logical Operator"};
	this.rtwnameHashMap["<S10>/Logical Operator1"] = {sid: "test:128"};
	this.sidHashMap["test:128"] = {rtwname: "<S10>/Logical Operator1"};
	this.rtwnameHashMap["<S10>/Product"] = {sid: "test:129"};
	this.sidHashMap["test:129"] = {rtwname: "<S10>/Product"};
	this.rtwnameHashMap["<S10>/Relational Operator"] = {sid: "test:130"};
	this.sidHashMap["test:130"] = {rtwname: "<S10>/Relational Operator"};
	this.rtwnameHashMap["<S10>/Relational Operator1"] = {sid: "test:131"};
	this.sidHashMap["test:131"] = {rtwname: "<S10>/Relational Operator1"};
	this.rtwnameHashMap["<S10>/count"] = {sid: "test:132"};
	this.sidHashMap["test:132"] = {rtwname: "<S10>/count"};
	this.rtwnameHashMap["<S10>/state"] = {sid: "test:133"};
	this.sidHashMap["test:133"] = {rtwname: "<S10>/state"};
	this.rtwnameHashMap["<S11>/u"] = {sid: "test:64:1"};
	this.sidHashMap["test:64:1"] = {rtwname: "<S11>/u"};
	this.rtwnameHashMap["<S11>/Compare"] = {sid: "test:64:2"};
	this.sidHashMap["test:64:2"] = {rtwname: "<S11>/Compare"};
	this.rtwnameHashMap["<S11>/Constant"] = {sid: "test:64:3"};
	this.sidHashMap["test:64:3"] = {rtwname: "<S11>/Constant"};
	this.rtwnameHashMap["<S11>/y"] = {sid: "test:64:4"};
	this.sidHashMap["test:64:4"] = {rtwname: "<S11>/y"};
	this.rtwnameHashMap["<S12>/u"] = {sid: "test:65:1"};
	this.sidHashMap["test:65:1"] = {rtwname: "<S12>/u"};
	this.rtwnameHashMap["<S12>/Compare"] = {sid: "test:65:2"};
	this.sidHashMap["test:65:2"] = {rtwname: "<S12>/Compare"};
	this.rtwnameHashMap["<S12>/Constant"] = {sid: "test:65:3"};
	this.sidHashMap["test:65:3"] = {rtwname: "<S12>/Constant"};
	this.rtwnameHashMap["<S12>/y"] = {sid: "test:65:4"};
	this.sidHashMap["test:65:4"] = {rtwname: "<S12>/y"};
	this.rtwnameHashMap["<S13>/u"] = {sid: "test:76:1"};
	this.sidHashMap["test:76:1"] = {rtwname: "<S13>/u"};
	this.rtwnameHashMap["<S13>/Compare"] = {sid: "test:76:2"};
	this.sidHashMap["test:76:2"] = {rtwname: "<S13>/Compare"};
	this.rtwnameHashMap["<S13>/Constant"] = {sid: "test:76:3"};
	this.sidHashMap["test:76:3"] = {rtwname: "<S13>/Constant"};
	this.rtwnameHashMap["<S13>/y"] = {sid: "test:76:4"};
	this.sidHashMap["test:76:4"] = {rtwname: "<S13>/y"};
	this.rtwnameHashMap["<S14>/u"] = {sid: "test:77:1"};
	this.sidHashMap["test:77:1"] = {rtwname: "<S14>/u"};
	this.rtwnameHashMap["<S14>/Compare"] = {sid: "test:77:2"};
	this.sidHashMap["test:77:2"] = {rtwname: "<S14>/Compare"};
	this.rtwnameHashMap["<S14>/Constant"] = {sid: "test:77:3"};
	this.sidHashMap["test:77:3"] = {rtwname: "<S14>/Constant"};
	this.rtwnameHashMap["<S14>/y"] = {sid: "test:77:4"};
	this.sidHashMap["test:77:4"] = {rtwname: "<S14>/y"};
	this.rtwnameHashMap["<S15>/u"] = {sid: "test:88:1"};
	this.sidHashMap["test:88:1"] = {rtwname: "<S15>/u"};
	this.rtwnameHashMap["<S15>/Compare"] = {sid: "test:88:2"};
	this.sidHashMap["test:88:2"] = {rtwname: "<S15>/Compare"};
	this.rtwnameHashMap["<S15>/Constant"] = {sid: "test:88:3"};
	this.sidHashMap["test:88:3"] = {rtwname: "<S15>/Constant"};
	this.rtwnameHashMap["<S15>/y"] = {sid: "test:88:4"};
	this.sidHashMap["test:88:4"] = {rtwname: "<S15>/y"};
	this.rtwnameHashMap["<S16>/u"] = {sid: "test:89:1"};
	this.sidHashMap["test:89:1"] = {rtwname: "<S16>/u"};
	this.rtwnameHashMap["<S16>/Compare"] = {sid: "test:89:2"};
	this.sidHashMap["test:89:2"] = {rtwname: "<S16>/Compare"};
	this.rtwnameHashMap["<S16>/Constant"] = {sid: "test:89:3"};
	this.sidHashMap["test:89:3"] = {rtwname: "<S16>/Constant"};
	this.rtwnameHashMap["<S16>/y"] = {sid: "test:89:4"};
	this.sidHashMap["test:89:4"] = {rtwname: "<S16>/y"};
	this.rtwnameHashMap["<S17>/u"] = {sid: "test:100:1"};
	this.sidHashMap["test:100:1"] = {rtwname: "<S17>/u"};
	this.rtwnameHashMap["<S17>/Compare"] = {sid: "test:100:2"};
	this.sidHashMap["test:100:2"] = {rtwname: "<S17>/Compare"};
	this.rtwnameHashMap["<S17>/Constant"] = {sid: "test:100:3"};
	this.sidHashMap["test:100:3"] = {rtwname: "<S17>/Constant"};
	this.rtwnameHashMap["<S17>/y"] = {sid: "test:100:4"};
	this.sidHashMap["test:100:4"] = {rtwname: "<S17>/y"};
	this.rtwnameHashMap["<S18>/u"] = {sid: "test:101:1"};
	this.sidHashMap["test:101:1"] = {rtwname: "<S18>/u"};
	this.rtwnameHashMap["<S18>/Compare"] = {sid: "test:101:2"};
	this.sidHashMap["test:101:2"] = {rtwname: "<S18>/Compare"};
	this.rtwnameHashMap["<S18>/Constant"] = {sid: "test:101:3"};
	this.sidHashMap["test:101:3"] = {rtwname: "<S18>/Constant"};
	this.rtwnameHashMap["<S18>/y"] = {sid: "test:101:4"};
	this.sidHashMap["test:101:4"] = {rtwname: "<S18>/y"};
	this.rtwnameHashMap["<S19>/u"] = {sid: "test:118:1"};
	this.sidHashMap["test:118:1"] = {rtwname: "<S19>/u"};
	this.rtwnameHashMap["<S19>/Compare"] = {sid: "test:118:2"};
	this.sidHashMap["test:118:2"] = {rtwname: "<S19>/Compare"};
	this.rtwnameHashMap["<S19>/Constant"] = {sid: "test:118:3"};
	this.sidHashMap["test:118:3"] = {rtwname: "<S19>/Constant"};
	this.rtwnameHashMap["<S19>/y"] = {sid: "test:118:4"};
	this.sidHashMap["test:118:4"] = {rtwname: "<S19>/y"};
	this.rtwnameHashMap["<S20>:54"] = {sid: "test:132:54"};
	this.sidHashMap["test:132:54"] = {rtwname: "<S20>:54"};
	this.rtwnameHashMap["<S20>:53"] = {sid: "test:132:53"};
	this.sidHashMap["test:132:53"] = {rtwname: "<S20>:53"};
	this.rtwnameHashMap["<S20>:16"] = {sid: "test:132:16"};
	this.sidHashMap["test:132:16"] = {rtwname: "<S20>:16"};
	this.rtwnameHashMap["<S20>:56"] = {sid: "test:132:56"};
	this.sidHashMap["test:132:56"] = {rtwname: "<S20>:56"};
	this.rtwnameHashMap["<S20>:28"] = {sid: "test:132:28"};
	this.sidHashMap["test:132:28"] = {rtwname: "<S20>:28"};
	this.rtwnameHashMap["<S20>:41"] = {sid: "test:132:41"};
	this.sidHashMap["test:132:41"] = {rtwname: "<S20>:41"};
	this.rtwnameHashMap["<S20>:57"] = {sid: "test:132:57"};
	this.sidHashMap["test:132:57"] = {rtwname: "<S20>:57"};
	this.rtwnameHashMap["<S20>:49"] = {sid: "test:132:49"};
	this.sidHashMap["test:132:49"] = {rtwname: "<S20>:49"};
	this.rtwnameHashMap["<S20>:27"] = {sid: "test:132:27"};
	this.sidHashMap["test:132:27"] = {rtwname: "<S20>:27"};
	this.getSID = function(rtwname) { return this.rtwnameHashMap[rtwname];}
	this.getRtwname = function(sid) { return this.sidHashMap[sid];}
}
RTW_rtwnameSIDMap.instance = new RTW_rtwnameSIDMap();

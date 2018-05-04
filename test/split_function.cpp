#include <iostream>
#include <string>
#include <vector>
#include <gov/dfs/daemon.h>
using namespace std;


//---------------------------Testing------------------------//
bool test(const string& input, const string& expected) {
	
	if( usgov::dfs::daemon::resolve_filename(input) != expected ){
		return false;
	}
	return true;
}

vector<int> testing_functions(){
	vector<int> report;
	int counter=0;
	
	if(!test("","")) report.push_back(counter);
	counter++;
	
	if(!test(" "," ")) report.push_back(counter);
	counter++;

	if(!test("43gfF","43/gf/F")) report.push_back(counter);
	counter++;
	
	if(!test(" 43gfF"," 4/3g/fF")) report.push_back(counter);
	counter++;
	
	if(!test("43gfF ","43/gf/F ")) report.push_back(counter);
	counter++;
	
	if(!test("43 gfF ","43/ g/fF/ ")) report.push_back(counter);
	counter++;
	
	if(!test("/43gfF","/4/3g/fF")) report.push_back(counter);
	counter++;
	
	if(!test("43gfF/","43/gf/F/")) report.push_back(counter);
	counter++;
	
	if(!test("/43gfF/","/4/3g/fF//")) report.push_back(counter);
	counter++;
	
	if(!test("/43/gfF/","/4/3//gf/F/")) report.push_back(counter);
	counter++;
	
	if(!test("/4 3/gf F/","/4/ 3//g/f /F/")) report.push_back(counter);
	counter++;
	
	if(!test("XbNzrbauge4avou2GSJaeXYti9f","Xb/Nz/rb/au/ge/4a/vo/u2/GS/Ja/eX/Yt/i9/f")) report.push_back(counter);
	counter++;
	
	if(!test("wWhf4YYacvxDxy4zMJDCPUrcT2","wW/hf/4Y/Ya/cv/xD/xy/4z/MJ/DC/PU/rc/T2")) report.push_back(counter);
	counter++;
	
	if(!test("Wwg5W8aJ9z2ZMkuGJTwirWo1jmP","Ww/g5/W8/aJ/9z/2Z/Mk/uG/JT/wi/rW/o1/jm/P")) report.push_back(counter);
	counter++;
	
	if(!test("WVN8wmzwZA8ju2YpyD3zBzRfJJr","WV/N8/wm/zw/ZA/8j/u2/Yp/yD/3z/Bz/Rf/JJ/r")) report.push_back(counter);
	counter++;
	
	if(!test("wkw9kTEkpruFoJcvPfrWkbP7z7B","wk/w9/kT/Ek/pr/uF/oJ/cv/Pf/rW/kb/P7/z7/B")) report.push_back(counter);
	counter++;
	
	if(!test("w3NyamRKPaUQWN49jbgv59fiftP","w3/Ny/am/RK/Pa/UQ/WN/49/jb/gv/59/fi/ft/P")) report.push_back(counter);
	counter++;
	
	if(!test("VRHsXd7sHKkPdqGwLQniwWrCV3c","VR/Hs/Xd/7s/HK/kP/dq/Gw/LQ/ni/wW/rC/V3/c")) report.push_back(counter);
	counter++;
	
	if(!test("vMJffYwL1XToRrMovDjdXQYnNgp","vM/Jf/fY/wL/1X/To/Rr/Mo/vD/jd/XQ/Yn/Ng/p")) report.push_back(counter);
	counter++;
	
	if(!test("VktUJ9KNu8nen9XbJJd2XeKr6SJ","Vk/tU/J9/KN/u8/ne/n9/Xb/JJ/d2/Xe/Kr/6S/J")) report.push_back(counter);
	counter++;

	if(!test("uZ2wbWAkXLampzZbUiUrMrdsfQN","uZ/2w/bW/Ak/XL/am/pz/Zb/Ui/Ur/Mr/ds/fQ/N")) report.push_back(counter);
	counter++;
		
	if(!test("v3r2wbw4S2RYeWW4qFJcdfF2jyx","v3/r2/wb/w4/S2/RY/eW/W4/qF/Jc/df/F2/jy/x")) report.push_back(counter);
	counter++;
	
	if(!test("vegCBMjENnAMVdsQuHSXDzJTbVf","ve/gC/BM/jE/Nn/AM/Vd/sQ/uH/SX/Dz/JT/bV/f")) report.push_back(counter);
	counter++;
	
	if(!test("vFGt4baH8iyZ6eSnGPSVaePSh6K","vF/Gt/4b/aH/8i/yZ/6e/Sn/GP/SV/ae/PS/h6/K")) report.push_back(counter);
	counter++;
	
	
	for (vector<int>::iterator it = report.begin(); it != report.end(); ++it)
      std::cout << ' ' << *it;
      std::cout << '\n';	
}





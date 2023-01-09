/*
 *   Copyright (c) 2022 Malte Hering
 *   All rights reserved.

 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at

 *   http://www.apache.org/licenses/LICENSE-2.0

 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#ifndef Debug_h
#define Debug_h
bool debug = false;
#define Debug(name,value) Serial.print(name); Serial.print(":"); Serial.print(value); Serial.print("\t"); debug=true;  

#define DebugValue(value) Serial.print(#value); Serial.print(":"); Serial.print(value); Serial.print("\t"); debug=true;

#define DebugStruct(name) Serial.println(""); Serial.print(#name); Serial.print(": {"); debug=true;  
#define DebugStructValue(value) if (true) { String name = String(#value); int index = name.indexOf("."); Serial.print(" ");  if (index > 0) { index++; Serial.print(name.substring(index)); } else { Serial.print(name); } Serial.print(":"); Serial.print(value);  debug=true;  }
#define DebugStructEnd() Serial.println(" }"); debug=true;  

#define DebugProgressStart(text) Serial.println(""); Serial.print(text); Serial.print(":"); debug=true;
#define DebugProgressUpdate(name) Serial.print("."); debug=true;
#define DebugProgressEnd(text) Serial.print(">"); Serial.print(text); Serial.print("\t"); debug=true;

#define loopDebug() if (debug) { Serial.println(); debug = false; }

#endif
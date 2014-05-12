You should have python installed. Tested and working with Python 3.3.2.

To add an external tool to Visual Studio, go to the [Tools] menu, and select [External Tools...].
Click the [Add] button, then fill in the fields as follows:
- add "[gus] cpplint" as the title.
- set the path to the python executable as the command. If you have it in the path, you can just use the exe name (E.g. "python.exe") (without quotes).  
- set the arguments to (with quotes): 
  "d:\work\GustysSerpents\Serpents\tools\cpplint\cpplint.py" --output=vs7 "$(ItemPath)"
- adjust it to your location of cpplint.py
- set the initial directory to "$(ProjectDir)" (without quotes). TODO: or is it "$(ItemDir)" ? Or does it not matter?
- enable "Use Output window"
- hit OK/Apply

Alternatively, if you don't plan on defining [VERA_ROOT], you could change the arguments to include -r, as in:
 --profile gus --vc-report - --warning --summary $(ItemPath) -r d:\work\GustysSerpents\Serpents\tools\vera
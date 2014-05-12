You should add the environment variable [VERA_ROOT] and have it point to this directory.
(The [scripts] subdirectory is expected to be found in [VERA_ROOT].)

To add an external tool to Visual Studio, go to the [Tools] menu, and select [External Tools...].
Click the [Add] button, then fill in the fields as follows:
- add [vera++] as the title.
- set the path to the vera++ executable as the command (E.g. "c:\Program Files (x86)\vera++\bin\vera++.exe") (without quotes).  
- set the arguments to: "--profile gus --vc-report - --warning --summary $(ItemPath)" (without quotes).
- set the initial directory to "$(ProjectDir)" (without quotes). TODO: or is it "$(ItemDir)" ? Or does it not matter?
- enable "Use Output window"
- hit OK/Apply

Alternatively, if you don't plan on defining [VERA_ROOT], you could change the arguments to include -r, as in:
 --profile gus --vc-report - --warning --summary $(ItemPath) -r d:\work\GustysSerpents\Serpents\tools\vera
import lit.formats
from platform import system

config.name = 'Ballerina codegen'
config.test_format = lit.formats.ShTest(True)

config.suffixes = ['.bal']

config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = os.path.join(config.my_obj_root, 'test')

if system() == 'Windows':
   nballerinacc_path = os.path.join(os.path.join(config.my_obj_root, 'Release'), 'nballerinacc')
   run_script = 'testRunScript.bat' 
else:
   nballerinacc_path = os.path.join(config.my_obj_root, 'nballerinacc')
   run_script = 'testRunScript.sh' 
 
config.substitutions.append(('%nballerinacc', nballerinacc_path))

config.substitutions.append(('%java_path',config.java_path))

config.substitutions.append(('%testRunScript',
    os.path.join(config.test_source_root, run_script)))

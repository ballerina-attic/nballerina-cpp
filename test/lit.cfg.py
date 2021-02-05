import lit.formats

config.name = 'Ballerina codegen'
config.test_format = lit.formats.ShTest(True)

config.suffixes = ['.bal']

config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = os.path.join(config.my_obj_root, 'test')

config.substitutions.append(('%nballerinacc',
    os.path.join(config.my_obj_root, 'nballerinacc')))

config.substitutions.append(('%java_path',config.java_path))

config.substitutions.append(('%testRunScript',
    os.path.join(config.test_source_root, 'testRunScript.sh')))

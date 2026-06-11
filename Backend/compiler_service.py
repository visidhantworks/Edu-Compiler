import subprocess
import tempfile
import json
import uuid
import os
import re
class CompilerService:

    def __init__(self):
        self.compiler_path = "./compiler"

    def analyze(self, source_code):

        filename = f"temp/{uuid.uuid4()}.c"

        os.makedirs("temp", exist_ok=True)

        with open(filename, "w") as f:
            f.write(source_code)
 
        try:

            result = subprocess.run(
                [self.compiler_path, filename],
                capture_output=True,
                text=True,
                timeout=10
            )

            ansi_escape = re.compile(
                r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])'
            )

            clean_stdout = ansi_escape.sub('', result.stdout)

            print("STDOUT:")
            print(clean_stdout)

            print("STDERR:")
            print(result.stderr)

            return {
                "success": result.returncode == 0,
                "stdout": clean_stdout,
                "stderr": result.stderr,
                "return_code": result.returncode
            }

        except subprocess.TimeoutExpired:
                return {
                "success": False,
                "message": "Compiler timeout"
                        }


        finally:

            if os.path.exists(filename):
                os.remove(filename)
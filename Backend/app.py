from flask import Flask, request, jsonify
from flask_cors import CORS
from compiler_service import CompilerService

app = Flask(__name__)
CORS(app, origins=[
    "https://your-netlify-site.netlify.app"
])

compiler = CompilerService()

@app.route("/api/analyze", methods=["POST"])
def analyze():

    data = request.get_json()

    code = data.get("code", "")

    if not code.strip():
        return jsonify({
            "success": False,
            "message": "No source code provided"
        }), 400

    result = compiler.analyze(code)
    print(result["stdout"])
    return jsonify(result)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8000)
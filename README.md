<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Questionário</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
        }
        .question {
            margin-bottom: 10px;
        }
        .button {
            margin-top: 20px;
            padding: 10px 20px;
            background-color: #4CAF50;
            color: white;
            border: none;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <h1>Questionário</h1>
    <form id="quizForm">
        <div class="question">
            <p>1. Qual é a capital do Brasil?</p>
            <label><input type="radio" name="q1" value="correct"> Brasília</label><br>
            <label><input type="radio" name="q1" value="incorrect"> Rio de Janeiro</label><br>
            <label><input type="radio" name="q1" value="incorrect"> São Paulo</label><br>
        </div>
        
        <div class="question">
            <p>2. Quem pintou a Mona Lisa?</p>
            <label><input type="radio" name="q2" value="incorrect"> Vincent van Gogh</label><br>
            <label><input type="radio" name="q2" value="correct"> Leonardo da Vinci</label><br>
            <label><input type="radio" name="q2" value="incorrect"> Pablo Picasso</label><br>
        </div>
        
        <div class="question">
            <p>3. Qual é o maior planeta do nosso sistema solar?</p>
            <label><input type="radio" name="q3" value="incorrect"> Terra</label><br>
            <label><input type="radio" name="q3" value="correct"> Júpiter</label><br>
            <label><input type="radio" name="q3" value="incorrect"> Marte</label><br>
        </div>
        
        <button type="button" class="button" onclick="checkAnswers()">Verificar Respostas</button>
    </form>
    
    <div id="result" style="display:none;">
        <h2>Resultado</h2>
        <p id="score"></p>
    </div>

    <script>
        function checkAnswers() {
            var totalQuestions = 3;
            var score = 0;
            
            // Check each question
            for (var i = 1; i <= totalQuestions; i++) {
                var question = document.querySelector('input[name="q' + i + '"]:checked');
                if (question) {
                    if (question.value === 'correct') {
                        score++;
                    }
                }
            }

            // Show the result
            document.getElementById('quizForm').style.display = 'none';
            document.getElementById('result').style.display = 'block';
            document.getElementById('score').innerText = 'Você acertou ' + score + ' de ' + totalQuestions + ' perguntas.';
        }
    </script>
</body>
</html>

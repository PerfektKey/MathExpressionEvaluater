import random

def generate_random_expression():
    operators = ['+', '-', '*']
    operands = [str(num) for num in range(-100, 100)]
    
    expression = ''
    
    # Generate a random number of terms
    num_terms = random.randint(10, 40)
    
    for i in range(num_terms):
        # Randomly choose an operator
        operator = random.choice(operators)
        
        # Randomly choose an operand
        operand = random.choice(operands)
        
        # Append the operator and operand to the expression
        expression += operand + ' ' + operator + ' '
    
    # Add the final operand
    expression += random.choice(operands)
    
    return expression

# Generate a random math expression
random_expression = generate_random_expression()
print(random_expression)

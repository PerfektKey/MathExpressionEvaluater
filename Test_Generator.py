import random

def generate_expression(operators, operands, depth):
    if depth == 0:
        return str(random.choice(operands))
    else:
        operator = random.choice(operators)
        if operator == '+':
            return f"({generate_expression(operators, operands, depth-1)} {operator} {generate_expression(operators, operands, depth-1)})"
        elif operator == '-':
            return f"({generate_expression(operators, operands, depth-1)} {operator} {generate_expression(operators, operands, depth-1)})"
        elif operator == '*':
            return f"{generate_expression(operators, operands, depth-1)} {operator} {generate_expression(operators, operands, depth-1)}"
        elif operator == '/':
            return f"{generate_expression(operators, operands, depth-1)} {operator} {generate_expression(operators, operands, depth-1)}"
        else:
            raise ValueError("Invalid operator")

def generate_random_expression(min_value, max_value, depth):
    operators = ['+', '-', '*', '/']
    operands = list(range(min_value, max_value+1))
    return generate_expression(operators, operands, depth)

expression = generate_random_expression(1, 10, 3)
print(expression)


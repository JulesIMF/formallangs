from os import system


def statename(state: int) -> str:
    return "state" + str(state)

with open("automata.txt") as src, open("automata.dot", 'w') as dst:
    print("digraph\n{\nrankdir=\"LR\";", file=dst)
    arrows=[]
    normal_states=set()
    finite_states=set()
    starting_state = None
    linenum = 0

    reading_arrows = True

    for line in src:
        if line[-1] == '\n':
            line = line[:len(line) - 1]

        linenum += 1
        if line == "": # switch to reading finite states
            reading_arrows = False
            if starting_state == None:
                starting_state = min(normal_states)

            continue
        
        if reading_arrows:
            tokens = line.split(' ')
            if '' in tokens:
                tokens.remove('')
            if len(tokens) < 2:
                print(f"Warning: line {linenum} is incorrect, less then 2 args; line skipped")
                continue

            try:
                first = int(tokens[0])
                second = int(tokens[1])
            
                transition = ""
                for i in range(2, len(tokens)):
                    if ("\"" in tokens[i]):
                        print(f"Warning: line {linenum} is incorrect, there is a transtion word containing double quotes; word skipped")
                    if (i > 2):
                        transition += ", "
                    transition += str(tokens[i])
                
                
                if (len(tokens) == 2):
                    transition = "ε"
                
                arrows.append((first, second, transition))
                normal_states.add(first)
                normal_states.add(second)
            
            except:
                print(f"Warning: line {linenum} is incorrect, first 2 args must be ints, then go transition words; line skipped")
        else:
            try:
                finites = []
                for w in line.split(' '):
                    finites.append(int(w))

                for state in finites:
                    if state in normal_states:
                        normal_states.remove(state)
                    elif state not in finite_states:
                        print(f"Warning: isolated state {state} declared finite; state skipped")
                        continue
                    
                    finite_states.add(state)
            except:
                print(f"Error: line {linenum} is incorrect, finite states must be ints; terminating")
                exit(-1)
    
    
    if starting_state != 0:
        print(f"Warning: starting state is not 0; assuming starting state {starting_state}")

    print(f"nowhere[label=\"\", shape=\"none\"];", file=dst)
    for state in normal_states:
        print(f"{statename(state)}[label={state}, shape=\"circle\"];", file=dst)
    
    for state in finite_states:
        print(f"{statename(state)}[label={state}, shape=\"doublecircle\"];", file=dst)

    for arrow in arrows:
        print(f"{statename(arrow[0])}->{statename(arrow[1])}[label=\"{arrow[2]}\"];", file=dst)
    
    print(f"nowhere->{statename(starting_state)};", file=dst)
    print("}", file=dst)

system("dot -Tpng automata.dot -o automata.png > /dev/null")

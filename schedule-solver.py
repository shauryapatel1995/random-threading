from ortools.linear_solver import pywraplp

# Create the mip solver with the SCIP backend.
solver = pywraplp.Solver.CreateSolver('SCIP')

infinity = solver.infinity()
# x and y are integer non-negative variables.
a = solver.IntVar(0.0, infinity, 'a')
x = solver.IntVar(0.0, infinity, 'x')
y = solver.IntVar(0.0, infinity, 'y')
b = solver.IntVar(0.0, infinity, 'b')
c = solver.IntVar(0.0, infinity, 'c')
e = solver.IntVar(0.0, infinity, 'e')
f = solver.IntVar(0.0, infinity, 'f')
g = solver.IntVar(0.0, infinity, 'g')
h = solver.IntVar(0.0, infinity, 'h')
i = solver.IntVar(0.0, infinity, 'i')
j = solver.IntVar(0.0, infinity, 'j')
k = solver.IntVar(0.0, infinity, 'k')

print('Number of variables =', solver.NumVariables())

# Thread types - 1. 
# 3 threads. 
# Possible combinations {1}, {1,1}, {1,1,1}

# x + 7 * y <= 17.5.
# Such a condition must exist for each thread type.
# A must represent a schedule and not a set. 
# For eg A = {{1}, {1}, {1}, {1}}, B = {{1,1}, {1}} and C = {{1,1,1}}
# That's completely crazy.
# a = {1}, x = {1}, y = {1}
# How to formulate for 2 thread types. 
# Suppose we want to schedule for two thread types. 
# 1 = 3 and 2 = 2. 
# we can still have sets as disjoint right?
# Constraint for three threads of type 1. 

# Formulating this is easy. Each set can have fixed number of given thread types added up to total threads for that type. 
solver.Add( a + h + i + 2 * j + 3 * k + 2 * b + 3 * c == 3)

# Constraing for two threads of type 2. 
# e, f, g are {2}, {2}, {2,2}
# but now we need {1,2}, {1,2,2}, {1,1,2}, {1,1,1,2,2} -> h, i, j, k
solver.Add( e + f + 2 * g + h + 2 * i + j + 2 * k == 2)

# x <= 3.5.
# Each set can be selected as 0 or 1. 
# This is trickier. What if we want to use a set more than once? then its value can incerase and that would decrease the number of variables used as well. 
# But this can be computationally expensive to calculate as well - but what if there is no constraint. The real constraint is just based on the number of threads.
# for each thread type. So maybe we can get rid of these constraints completely. x and y become repititve because of this because 3 a basically means a + a + a. 
# That is schedule a sequentially 3 times. 
# Translating to a schedule might be more difficult for above approach because a = 3 = {{1}, {1}, {1}} - but not that difficult. 

# solver.Add(a <= 1)
# solver.Add(b <= 1)
# solver.Add(c <= 1)
# solver.Add(x <= 1)
# solver.Add(y <= 1)
# solver.Add(e <= 1)
# solver.Add(f <= 1)
# solver.Add(g <= 1)
# solver.Add(h <= 1)
# solver.Add(i <= 1)
# solver.Add(j <= 1)
# solver.Add(k <= 1)

print('Number of constraints =', solver.NumConstraints())

# Maximize x + 10 * y.
solver.Minimize( a + 8.4 * b + 16 * c + 124 * e + 124 * f + 138 * g + 113 * h + 145 * i + 125 * j + 200 * k)

status = solver.Solve()

if status == pywraplp.Solver.OPTIMAL:
    print('Solution:')
    print('Objective value =', solver.Objective().Value())
    print('a =', a.solution_value())
    print('x =', x.solution_value())
    print('y =', y.solution_value())
    print('b =', b.solution_value())
    print('c =', c.solution_value())
    print('e =', e.solution_value())
    print('f =', f.solution_value())
    print('g =', g.solution_value())
    print('h =', h.solution_value())
    print('i =', i.solution_value())
    print('j =', j.solution_value())
    print('k =', k.solution_value())
else:
    print('The problem does not have an optimal solution.')


# How do we optimally determine the weights?
# One way is that we bound the degrdations using sum of all other threads onto the longest thread for that set 
# So for example (t1 + t2)*t3 where t3 is time of thread 3. 
# Another way might be trying to predict the degradation?
# lets say we have a group of threads -> {1,2,3} => expected vs actual. 
# This seems to be a regression problem? 
# Sample random runs and update weights maybe?
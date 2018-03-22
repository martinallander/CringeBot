class robot_data:
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y
    def __str__(self):
        return "x: " + str(self.x) + " and y: " + str(self.y)
    
rd = robot_data(1,2)
print(rd)

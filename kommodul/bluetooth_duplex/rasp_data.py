class robot_data:
    def __init__(self, x=0, y=0, stop = False):
        self.x = x
        self.y = y
	self.stop = stop
    def __str__(self):
        return "x: " + str(self.x) + " and y: " + str(self.y)
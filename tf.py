#!/usr/bin/env python


import rospy
import tf
import math
from std_msgs.msg import String

class myNode:

	def __init__(self):

		rospy.init_node('listenToMyTF', anonymous=True)
		self.listener = tf.TransformListener()
		self.rate = rospy.Rate(5)
        	rospy.Subscriber("moves", String, self.callback)
		self.coordinates = []

	def callback(self, data):
		try:
			t = self.listener.getLatestCommonTime("/map", "/base_link")
			(pose, quaternion) = self.listener.lookupTransform('/map', '/base_link', t)
			euler = tf.transformations.euler_from_quaternion(quaternion)
			list = []
			x = round(pose[0], 3)
			y = round(pose[1], 3)
			th_raw = euler[2]
			th = (euler[2] *180/math.pi)
			list.append(x)
			list.append(y)
			self.coordinates.append(list)
			
			#self.determine_direction(th)
			self.determine_home(th_raw)
			#print(self.coordinates)
			print("x: %s y: %s th: %s" % (x, y, th))
		except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
			pass

	def determine_direction(self, angle):
		if len(self.coordinates) < 2:
			pass
		else:
			latest_x = self.coordinates[len(self.coordinates)-1][0]
			latest_y = self.coordinates[len(self.coordinates)-1][1]
			older_x = self.coordinates[len(self.coordinates)-2][0]
			older_y = self.coordinates[len(self.coordinates)-2][1]
			if latest_x  + 0.08*math.cos(angle) > older_x:
				print("go forward! (x)")
			else:
				pass
			if abs(latest_y + 0.08*math.sin(angle)) > abs(older_y):
				print("go forward! (y)")
			else:
				pass 
	
	def determine_home(self, angle):
		latest_x = self.coordinates[len(self.coordinates)-1][0]
		latest_y = self.coordinates[len(self.coordinates)-1][1]
		older_x = self.coordinates[len(self.coordinates)-2][0]
		older_y = self.coordinates[len(self.coordinates)-2][1]
		next_x = latest_x + 0.08*math.cos(angle)
		next_y = latest_y + 0.08*math.sin(angle)
		angle_deg = math.degrees(angle)
		atan_deg = math.degrees(math.atan2(latest_x, latest_y))
		
		if abs(latest_x -0) < 0.20  and abs(latest_y -0) < 0.20:
			print("You're home!")
		else:
			pass

		if latest_x > 0 and latest_y > 0:
			beta = 90 - atan_deg
			if abs(beta + abs(angle_deg) - 180) < 15:
				print("looking the right way!")
		elif latest_x > 0 and latest_y < 0:
			beta = atan_deg - 90
			if abs(beta + angle_deg - 180) < 10:
				print("looking the right way!")	
		elif latest_x < 0 and latest_y > 0:
			beta = 90 - abs(atan_deg)
			if angle_deg < 0 and beta - abs(angle_deg) < 10:
				print("looking the right way!")
		elif latest_x < 0 and latest_y < 0:
			beta = abs(atan_deg) - 90
			print(beta + angle_deg - 90)
			if angle_deg > 0 and beta + angle_deg - 90 < 10:	
				pass
			

	def run(self):
		#self.some_other_method()
		rospy.spin()

	

def main():
	listen = myNode()
	listen.run()
	

if __name__ == '__main__':
	try:
		main()
	except rospy.ROSInterruptException:
		pass


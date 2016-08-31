#	import MNIST datasets
import input_data	# via pip install mnist
mnist = input_data.read_data_sets("/tmp/data/", one_hot=True)

import tensorflow as tf 

# 	set hyper parameters

#	learning rate defines how fast we should update our weights
#		if learning rate is too big, we might skip our optimal solution
#		if it's too small, we might need too many iterations to reach
#			the optimal solution
#		.01 is a known decent learning rate for this problem
learning_rate = .01
training_iteration = 30
batch_size = 100
display_step = 2

# 	create model
#	TF Graph input, two placeholder operations
# 	these are variables that we will assign data to eventually
#		x = 2D tensor of floats, 784 = dimensionality of a flattened mnist image
#		y = corresponding mnist image class
x = tf.placeholder("float", [None, 784]) # mnist data image of shape 28*28=784
y = tf.placeholder("float", [None, 10]) # 0-9 digits recognition => 10 classes

# 	set model weights
#	these weights reflect probability of how data flows in the graph
#	updated continuously during training
#	the bias lets us shift our regression line to better fit the data
W = tf.Variable(tf.zeros([784, 10]))
b = tf.Variable(tf.zeros([10]))

# 	scopes help us name nodes in the graph visualizer called tensorboard
with tf.name_scope("Wx_b") as scope:
	# 	construct linear model
	#	logistic regression by multiplying inputs by weights and adding bias
	model = tf.nn.softmax(tf.matmul(x, W) + b) # softmax

# 	add summary ops to collect data
w_h = tf.histogram_summary("wights", W)
w_b = tf.histogram_summary("biases", b)

# 	in this second scope we will create our cost function
#	the cost function helps us minimize our error during training
with tf.name_scope("cost_function") as scope:
	#	minimize error using cross entropy
	#	this is a very popular function
	cost_function = -tf.reduce_sum(y*tf.log(model))

	#	create a summary to monitor the cost function during training
	#	so we can visualize it later
	tf.scalar_summary("cost_function", cost_function)

#	training function to improve accuracy
#	takes learning rate to set the pacing, and a cost function to minimize error rate
with tf.name_scope("train") as scope:
	#	gradient descent
	optimizer = tf.train.GradientDescentOptimizer(learning_rate).minimize(cost_function)

# 	initialize all the variables
init = tf.initialize_all_variables()

#	merge all summaries into a single operator
# 	this is very lazy, good for learning though!!
merged_summary_op = tf.merge_all_summaries()

# 	launch graph by initializing session to execute our dataflow grpah
with tf.Session() as sesh:
	sesh.run(init)

	# 	set the logs writer to the folder /tmp/tensorflow_logs
	summary_writer = tf.train.SummaryWriter('/Users/DavidHarmon/Developer/logs', 
											graph_def=sesh.graph_def)

	#	training cycle
	for iteration in range(training_iteration):
		avg_cost = 0
		total_batch = int(mnist.train.num_examples/batch_size)

		# 	loop over all batches
		for i in range(total_batch):
			batch_xs, batch_ys = mnist.train.next_batch(batch_size)

			#	fit training using batch data and gradient descent model for back propagation
			sesh.run(optimizer, feed_dict={x: batch_xs, y: batch_ys})

			#	compute the average loss
			avg_cost += sesh.run(cost_function, 
					feed_dict={x: batch_xs, y: batch_ys}) / total_batch

			# 	write logs for each iteration
			summary_str = sesh.run(merged_summary_op, 
				feed_dict={x: batch_xs, y: batch_ys})
			summary_writer.add_summary(summary_str, iteration * total_batch + i)

		#	display logs per display step to make sure our model is improving
		if iteration % display_step == 0:
			print "Iteration: ", '%04d' % (iteration + 1), 'Cost=', '{:.9f}'.format(avg_cost)

	print 'Training Complete!'

	#	test the model by comparing model values to output values
	predictions = tf.equal(tf.argmax(model, 1), tf.argmax(y, 1))

	#	calculate accuracy
	accuracy = tf.reduce_mean(tf.cast(predictions, 'float'))
	print 'Accuracy:', accuracy.eval({x: mnist.test.images, y: mnist.test.labels})



import tensorflow as tf 
import numpy as np 
import input_data

mnist_width = 28	# each char image is 28x28
n_visible = mnist_width * mnist_width	# number of input nodes
n_hidden = 500	# generally should have 2/3 hidden nodes as input nodes
corruption_level = 0.3	# want to purposefully corrupt data later to improve

# create node for input data
X = tf.placeholder('float', [None, n_visible], name='X')

# create node for corruption mask
mask = tf.placeholder('float', [None, n_visible], name='mask')

# create nodes for hidden variables
W_init_mask = 4 * np.sqrt(6. / (n_visible + n_hidden))

# intialize weights
W_init = tf.random_uniform(shape=[n_visible, n_hidden],
						   minval=-W_init_mask,
						   maxval=W_init_mask)

# intialize hidden layer
W = tf.Variable(W_init, name='W')
b = tf.Variable(tf.zeros([n_hidden]), name='b')

W_prime = tf.transpose(W) #	tied weights between encoder and decoder
b_prime = tf.Variable(tf.zeros([n_visible]), name='b_prime')

def model(X, mask, W, b, W_prime, b_prime):
	tilde_X = mask * X 	# corrupted X

	Y = tf.nn.sigmoid(tf.matmul(tilde_X, W) + b)  # hidden state
	Z = tf.nn.sigmoid(tf.matmul(Y, W_prime) + b_prime)  # reconstructed input
	return Z

# build model graph
Z = model(X, mask, W, b, W_prime, b_prime)

# create cost function
cost = tf.reduce_sum(tf.pow(X-Z, 2)) # minimize squared error
train_op = tf.train.GradientDescentOptimizer(0.02).minimize(cost)

# load MNIST data, one_hot is a bit wise op to make computation faster
mnist = input_data.read_data_sets('MNIST_data/', one_hot=True)
trX, trY, teX, teY = mnist.train.images, mnist.train.labels, mnist.test.images, mnist.test.labels

# launch the graph in a session
with tf.Session() as sesh:
	# you need to initialize all variables
	tf.initialize_all_variables().run()

	for i in range(100):
		# for every image, retrieve input
		for start, end in zip(range(0, len(trX), 128), range(128, len(trX), 128)):
			input_ = trX[start:end]
			# create mask with the binomial distribution
			mask_np = np.random.binomial(1, 1 - corruption_level, input_.shape)
			# run training data on input we defined earlier
			sesh.run(train_op, feed_dict={X: input_, mask: mask_np})

		# create mask for the outer loop too
		mask_np = np.random.binomial(1, 1 - corruption_level, teX.shape)
		# print out results as we go along
		print(i, sesh.run(cost, feed_dict={X: teX, mask: mask_np}))

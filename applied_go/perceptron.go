/*
 *	In a neural net, you have artificial neurons as nodes in a graph
 *	representing some computation and you have flows between each node.
 *	These flows only fire their output to the next node if they pass
 *	a certain threshold, just like synapses in the human brain.
 *
 *	In order to fire, our artificial synapses nead an activation function
 *	to decide whether or not to fire given a specific input.  The most
 *	basic form of this is a binary functionhas only two possible outputs, 
 *	this is called a Heaviside Step Function.
 *	
 *	This is useful for classification problems where we need to decide if
 *	an input is a member of something or it is not.
 */

/* 
 *	requires:
 *	go get github.com/appliedgo/perceptron/draw
 */

 package main

import (
	"fmt"
	"math/rand"
	"time"

	"github.com/appliedgo/perceptron/draw"
)

/*
 *	Define a perceptron, initially is uses random weights and biases,
 *	but these will change and become more accurate during training.
 */

type Perceptron struct {
	weights []float32
	bias    float32
}

/*
 *	This is a Heaviside Step Function
 */
func (p *Perceptron) heaviside(f float32) int32 {
	if f < 0 {
		return 0
	}
	return 1
}

/*
 *	Create a new perceptron with n inputs, weights and biases are 
 *	initialized with random values between -1 and 1
 */
 func NewPerceptron(n int32) *Perceptron {
	var i int32
	w := make([]float32, n, n)
	for i = 0; i < n; i++ {
		w[i] = rand.Float32()*2 - 1
	}
	return &Perceptron{
		weights: w,
		bias:    rand.Float32()*2 - 1,
	}
}

/*
 *	Process implements the core functionality of the perceptron. 
 *	It weighs the input signals, sums them up, adds the bias, and
 *	runs the result through the Heaviside Step function. (The return 
 *	value could be a boolean but is an int32 instead, so that we can 
 *	directly use the value for adjusting the perceptron.)
 */
func (p *Perceptron) Process(inputs []int32) int32 {
	sum := p.bias
	for i, input := range inputs {
		sum += float32(input) * p.weights[i]
	}
	return p.heaviside(sum)
}

/*
 *	During the learning phase, the perceptron adjusts the weights
 *	and bias based on how much the perceptron's answer deviates from
 *	the true answer
 */
func (p *Perceptron) Adjust(inputs []int32, delta int32, learningRate float32) {
	for i, input := range inputs {
		p.weights[i] += float32(input) * float32(delta) * learningRate
	}
	p.bias += float32(delta) * learningRate
}

/*
 *	Training.  If we rule out the case where our line is vertical, then
 *	we should specify our line as a simle y = mx + b format.  
 *
 *	a and b specify the linear function that describes the separation line; 
 *	see below for details. They are defined at global level because we need 
 *	them in several places and I do not want to clutter the parameter lists 
 *	unnecessarily.
 */
var (
 	a, b int32
)

/* This function describes the separation line */
func f(x int32) int32 {
	return a*x + b
}

/*
 *	This helper functionreturns 1 if the solution is above our line, 
 *	and 0 if it is below
 */
func isAboveLine(point []int32, f func(int32) int32) int32 {
	x:=point[0]
	y:=point[1]
	if y > f(x) {
		return 1
	}
	return 0
}

/*
 *	Now it's time to train our model, the function generates random
 *	test points and feeds them to the perceptron, then the teacher
 *	compares the solution against it's solution manual and tells the
 *	perceptron how far off it is.
 */
func train(p *Perceptron, iters int, rate float32) {
	for i := 0; i < iters; i++ {
		// Generate a random point between -100 and 100.
		point := []int32{
			rand.Int31n(201) - 101,
			rand.Int31n(201) - 101,
		}

		actual := p.Process(point)
		expected := isAboveLine(point, f)
		delta := expected - actual

		p.Adjust(point, delta, rate)
	}
}

/*
 *	Now we try again, throw random points at it, but this time we don't
 *	tell it how far off it is and see if it classifies each point correctly
 */

/*
 *	This is our test function that tells us how we did correct/total
 */
func verify(p *Perceptron) int32 {
	var correctAnswers int32 = 0

	c := draw.NewCanvas()

	for i := 0; i < 100; i++ {
		point := []int32{
			rand.Int31n(201) - 101,
			rand.Int31n(201) - 101,
		}

		result := p.Process(point)
		if result == isAboveLine(point, f) {
			correctAnswers += 1
		}

		c.DrawPoint(point[0], point[1], result == 1)
	}

	c.DrawLinearFunction(a, b)
	c.Save()	// saves image as ./result.png

	return correctAnswers
}

func main() {
	rand.Seed(time.Now().UnixNano())

	a = rand.Int31n(11) - 6
	b = rand.Int31n(101) - 51

	p := NewPerceptron(2)

	iterations := 1000
	var learningRate float32 = 0.1 // Allowed range: 0 < learning rate <= 1.

	train(p, iterations, learningRate)

	successRate := verify(p)
	fmt.Printf("%d%% of the answers were correct.\n", successRate)
}

# clock
I implemented a simple two-button clock from scratch. I also wired up the display circuit connected to my raspberry pi.


The left button is the "blue" button in my comments (hooked up to GPIO 3) and the right button is the "white" button in my comments (hooked up to GPIO 2).

Clicking the white button starts the timer as normal. Clicking the blue button enters a little sequence of flashing 0s that will continue until the blue button is pressed again. You may then click the white button to increment "seconds". Click the blue button to move over to tens of seconds. The blue button goes to the next digit, while the white button increments the digit you are currently on. Once you hit the blue button four times, the clock begins as normal from your custom set time.

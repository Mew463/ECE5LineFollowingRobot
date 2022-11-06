void remoteControl() {
  int speed = map(IBus.readChannel(4), 1000,2000, 30, 255);
  int turnlimit = map(IBus.readChannel(5), 1000,2000, 30, 255);

  int forward = map(IBus.readChannel(1), 1000, 2000, -speed, speed);
  int turn = map(IBus.readChannel(3), 1000, 2000, -turnlimit, turnlimit);

  // Start Motors in forward direction
  runMotorAtSpeed(RIGHT, forward - turn);
  runMotorAtSpeed(LEFT, forward + turn);
}


#!/usr/bin/perl
use strict;
use Device::SerialPort;

while (1) {
	my $port = Device::SerialPort->new("/dev/ttyACM0");
	if ( ! $port ) {
		print "Marche pas, next\n";
		$port = Device::SerialPort->new("/dev/ttyACM1");
		if (! $port ) { print "Cannot open 0 nor 1 , ABORT\n"; }
		else { print "Opened ACM1"; }
	} else { print "Opened ACM0"; }
	$port->databits(8);
	$port->baudrate(9600); # <-- match to arduino settings
	$port->parity("none");
	$port->stopbits(1);
	$port->dtr_active(0);
	my $in;
	while ( $port )
	{
		$in = $port->input;
		print $in ;
		sleep (0.5);
	}

}

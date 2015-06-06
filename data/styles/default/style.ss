listmenu {
	background: #555;
	item {
		color: #ddd;
		background: #7000;
	}
	item!selected {
		color: #333;
		background: gradient(vert, #fff, #ccc, #ddd), image(flair-l.png)@left, image(flair-r.png)@right, [#8fff,0,0,60%,0];
	}
	group {
		color: #fff;
		background: gradient(vert, #888 #444 #000), [#6fff,1,0,60%,0];
	}
}

header {
	background: gradient(vert, #888 #444 #000), [#6fff,1,0,60%,0];
	color: #fff;
	clock {
		background: image(clock.png);
	}
}

button {
	background: gradient(vert, #888 #444 #000), [#6fff,1,0,60%,0];
	color: #fff;
	border: #777;
	padding: 0 10;
}

frame {
	background: #555;
	border: #333;
	padding: 10;
}

dialog {
	background: gradient(vert, #fff, #ccc, #ddd), [#8fff,0,0,60%,0];
	border: #333;
	color: #333;
}

test {
	geometry: 60%x40%+20%+30%;
	button!accept {
		geometry: 40x20-10-5;
	}
	button!deny {
		geometry: 40x20+10-5;
	}
	label {
		geometry: 130x20+10+5;
	}
}

music {
	player {
		background: #d333;
		song {
			color: #fff;
			geometry: 50%x77-10+10;
			scroll: true;
		}
		progressbar {
			geometry: 90%x18+5%-10;
			color: #000;
			background: gradient(vert, #888 #444 #000), [#6fff,1,1,1,1];
			fill: #8600, [image(stripe.png)!repeat-x@left,1,1,1,1];
		}
		artwork {
			geometry: 77x77+15+10;
			background: image(cd.png);
		}
	}
}

// vi: ts=4 sw=4 syntax=css;

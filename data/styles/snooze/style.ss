listmenu {
	background: image(bg.jpg)!fit;
	item {
		color: #ddd;
		background: #3000;
	}
	item!selected {
		color: #fff;
		background: #7000;
	}
	group {
		color: #fff;
		background: gradient(horiz, #035 #446 #248), gradient(vert, #8248 #8446 #8035), [#1fff,1,1,55%,1];
	}

}

header {
	background: gradient(horiz, #248 #446 #035), gradient(vert, #8248 #8446 #8035), [#1fff,1,1,55%,1];
}

music {
	player {
		background: image(bg.jpg)!fit, #6fff;
		song {
			font: snap;
			geometry: 50%x77-10+10;
			align: right;
			scroll: true;
			color: #000;
		}
		seekbar {
			color: #d700;
			geometry: 90%x20+5%-10;
			background: [image(bar-mid.png)!repeat-x,0,3,0,5], image(bar-lt.png)@left, image(bar-rt.png)@right;
			fill: [image(fill.png)!repeat-x@left,1,2,1,2], [image(fill-cap.png)@left,2,1,2,1], [image(fill-cap.png)@right,2,1,2,1], [#3e66,1,2,1,2];
			
		}
		progressbar {
			color: #d000;
			geometry: 90%x20+5%-10;
			background: [image(bar-mid.png)!repeat-x,0,3,0,5], image(bar-lt.png)@left, image(bar-rt.png)@right;
			fill: [image(fill.png)!repeat-x@left,1,2,1,2], [image(fill-cap.png)@left,2,1,2,1], [image(fill-cap.png)@right,2,1,2,1];
		}
		artwork {
			geometry: 77x77+15+10;
			background: [image(cd.png)!fit,0,0,0,-5];
			foreground: [image(case.png)!fit,0,0,0,-5];
		}
	}
}

calculator {
	background: #40000000;
	field {
		color: #000;
		background: gradient(vert, #e8f7fe #ade1fe #84d2fe #bfe6fd);
	}
	button {
		color: #fff;
		background: gradient(vert, #595959 #3f3f3f #2c2c2c #484848);
	}
	button!selected {
		color: #ff9000;
		background: gradient(vert, #6a6a6a #505050 #3d3d3d #595959);
	}
}

// vi: ts=4 sw=4 syntax=css;

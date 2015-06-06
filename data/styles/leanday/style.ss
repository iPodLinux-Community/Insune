listmenu {
	background: image(bg.png)!fit;
	item {
		color: #ddd;
		background: #7000;
	}
	item!selected {
		color: #333;
		background: image(item.png)!fit;
	}
	group {
		color: #fff;
		background: gradient(vert, #18a, #17a), [#18a,2,0,60%,0];
	}
}

header {
	color: #fff;
	background: gradient(vert, #18a, #17a), [#18a,2,0,60%,0];
}

// vi: ts=4 sw=4 syntax=css;

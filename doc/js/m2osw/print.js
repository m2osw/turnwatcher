/*
 * Force a call to print() to open the print dialog for customers
 * to just hit a button on your page and the page to go to the
 * printer.
 *
 * The following function is from Yahoo!
 * They may not have been the people who found out that you need
 * to hit the offsetHeight to then be able to print correctly,
 * but that the source of this function.
 *
 * To get the Print() dialog automatically, use the following:
 *
 * <body onload="Print();">...</body>
 */

function Print() {
	// The following line is required to show the print dialog after
	// mail is rendered. Affects IE browser.
	document.body.offsetHeight;
	window.print();
}

// vim: ts=4 sw=4

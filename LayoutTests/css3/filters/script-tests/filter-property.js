description("Test the parsing of the -webkit-filter property.");

// add a -webkit-filter property to the start of the stylesheet
document.styleSheets.item(0).addRule("body", "-webkit-filter: url(#a) url(#b)", 0);

var cssRule = document.styleSheets.item(0).cssRules.item(0);

shouldBe("cssRule.type", "1");

var declaration = cssRule.style;
shouldBe("declaration.length", "1");
shouldBe("declaration.getPropertyValue('-webkit-filter')", "'url(#a) url(#b)'");

successfullyParsed = true;

const fs = require('fs');
const { JSDOM } = require('jsdom');
const request = require('request');

const download = (url, path, callback) => {
	request.head(url, (err, res, body) => {
		request(url).pipe(fs.createWriteStream(path))
	})
}

(async function () {
	if (fs.existsSync("../data/Killers")) return;
	// Grab webpage
	const dom = await JSDOM.fromURL("https://deadbydaylight.fandom.com/wiki/Killer_Perks");
	const { document } = dom.window;
	// First element is apparently thead (jsdom is wack)
	// Grab all rows in table
	perks = [...document.querySelector("tbody").children].slice(1)
		.map((x) => {
			// Remap each row into object
			return {
				filename: x.querySelectorAll("a")[0].href.replace(/\/revision\/latest.+/, ""),
				name: x.querySelectorAll("a")[1].title.replaceAll(":", "").normalize("NFD").replace(/[\u0300-\u036f]/g, ""),
				// Description is URI encoded for simplicity
				killer: x.children[3].querySelectorAll("a")[0]?.textContent?.normalize("NFD")?.replace(/[\u0300-\u036f]/g, "")
			}
		});
	groupedPerks = perks.reduce((acc, value) => {
		if (!acc[value.killer]) {
			acc[value.killer] = [];
		}
		acc[value.killer].push(value);
		return acc;
	}, {});
	fs.mkdirSync("../data/Killers");
	for (let killer in groupedPerks) {
		killerName = killer == 'undefined' ? 'CommonKiller' : killer;
		fs.mkdirSync(`../data/Killers/${killerName}`);
		for (perk of groupedPerks[killer]) {
			download(perk.filename, `../data/Killers/${killerName}/${perk.name}.png`);
		}
	}
}());

(async function () {
	if (fs.existsSync("../data/Survivors")) return;
	// Grab webpage
	const dom = await JSDOM.fromURL("https://deadbydaylight.fandom.com/wiki/Survivor_Perks");
	const { document } = dom.window;
	// First element is apparently thead (jsdom is wack)
	// Grab all rows in table
	perks = [...document.querySelector("tbody").children].slice(1)
		.map((x) => {
			// Remap each row into object
			return {
				filename: x.querySelectorAll("a")[0].href.replace(/\/revision\/latest.+/, ""),
				name: x.querySelectorAll("a")[0].title.replaceAll(":", "").normalize("NFD").replace(/[\u0300-\u036f]/g, ""),
				// Description is URI encoded for simplicity
				survivor: x.children[3].querySelector("a")?.title?.replaceAll("\"", "'")?.normalize("NFD")?.replace(/[\u0300-\u036f]/g, "")
			}
		});
	groupedPerks = perks.reduce((acc, value) => {
		if (value.survivor == undefined) value.survivor = "CommonSurvivor";
		if (!acc[value.survivor]) {
			acc[value.survivor] = [];
		}
		acc[value.survivor].push(value);
		return acc;
	}, {});
	if (!fs.existsSync("data")) fs.mkdirSync("data");
	fs.mkdirSync("../data/Survivors");
	for (survivor in groupedPerks) {
		survivorName = survivor == 'undefined' ? 'CommonSurvivor' : survivor;
		fs.mkdirSync(`../data/Survivors/${survivorName}`);
		for (perk of groupedPerks[survivor]) {
			download(perk.filename, `../data/Survivors/${survivorName}/${perk.name}.png`);
		}
	}
}());
#!/usr/bin/env node
import fs from 'node:fs';
import path from 'node:path';

const args = process.argv.slice(2);
if (args.length !== 2) {
	console.error(`Wrong arguments`);
	process.exit(2);
}

const PROJECT_ID = args[0];
const TARGET_DIR = args[1];
const SRC_DIR = path.resolve('Projects', PROJECT_ID);

if (!fs.existsSync(SRC_DIR) || !fs.statSync(SRC_DIR).isDirectory()) {
	console.error(`Source project directory not found: ${SRC_DIR}`);
	process.exit(3);
}

fs.mkdirSync(TARGET_DIR, { recursive: true });

console.log(`Cleaning target directory: ${TARGET_DIR}`);

const targetEntries = fs.readdirSync(TARGET_DIR);

for (const entryName of targetEntries) {

	if (entryName === 'meta.ini') continue;
	const dstEntry = path.join(TARGET_DIR, entryName);
	console.log(`	- Removing ${dstEntry}`);
	fs.unlinkSync(dstEntry);
}

console.log(`Linking project contents from: ${SRC_DIR} -> ${TARGET_DIR}`);

const folders = ['Scripts', 'Meshes', 'Textures', 'Materials', 'Interface', 'F4SE', 'MCM'];

const sourceEntries = fs.readdirSync(SRC_DIR);

for (const entryName of sourceEntries) {
	const srcEntry = path.join(SRC_DIR, entryName);
	const dstEntry = path.join(TARGET_DIR, entryName);

	if (folders.includes(entryName)) {
		console.log(`	- Linking Folder ${entryName}`);


		fs.symlinkSync(srcEntry, dstEntry, 'dir');
	}

	const ext = path.extname(entryName).toLowerCase();

	if (ext === '.esp' || ext === '.esm' || ext === '.esl') {
		console.log(`	- Linking Plugin ${entryName}`);

		fs.symlinkSync(srcEntry, dstEntry, 'file');
	}
}


console.log('Deploy complete.');
process.exit(0);

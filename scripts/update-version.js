const fs = require('fs');
const path = require('path');
const git = require('git-rev-sync');

// Path to package.json
const packageJsonPath = path.join(__dirname, '../package.json');

// Read package.json
const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'));

// Get the latest Git tag or commit hash
const gitVersion = git.tag() || git.short(); // You can use git.long() for full hash

// Update the version field in package.json
packageJson.version = gitVersion;

// Write the updated package.json back to the file system
fs.writeFileSync(packageJsonPath, JSON.stringify(packageJson, null, 2));

console.log(`Updated version to: ${gitVersion}`);

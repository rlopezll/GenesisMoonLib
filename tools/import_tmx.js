const { parseXml } = require('@rgrove/parse-xml');
const fs = require('fs');
const path = require('node:path'); 

let filename = '';
let filename_out_h = '';
let filename_out_c = '';
let prefix = '';
if(process.argv.length > 5) {
    filename = process.argv[2];
    filename_out_h = process.argv[3];
    filename_out_c = process.argv[4];
    prefix = process.argv[5];
}
else {
    console.log(`node import_tmx.js <filename_tmx> <filename_output_header> <filename_output_c> <varname_prefix>`);
    process.exit(1);
}

const only_filename_out_header = path.basename(filename_out_h);

if (!fs.existsSync(filename)) {
    console.log(`Error: TMX file: ${filename} not exists.`);
    process.exit(1);
}

const XMLdata = fs.readFileSync(filename, { encoding: 'utf8', flag: 'r' });

const jObj2 = parseXml(XMLdata);
// let json2 = JSON.stringify(jObj2, null, 2);
// console.log(json2);

let enumTypesArray = [];
let tmx_objects_count = 0;

function parserEnumTypesObj(obj) {
    if(obj.name && obj.type) {
        if(obj.type == "element" && obj.name == "object" && obj.attributes && obj.attributes.type)
        {
            ++tmx_objects_count;
            enumTypesArray.push(obj.attributes.type);
        }
    }

    if(obj.children && Array.isArray(obj.children)) {
        obj.children.forEach(element => {
            parserEnumTypesObj(element);
        });
    }
}
parserEnumTypesObj(jObj2);

const enumTypes = [ ...new Set(enumTypesArray)]  

let headerFile = `#ifndef _TMX_${prefix}_OBJECTS_
#define _TMX_${prefix}_OBJECTS_

#include <genesis.h>
#include "moon/objects_game.h"

typedef enum {\n`;
let bFirstEnum = true;
enumTypes.forEach(element => {
    if(!bFirstEnum) {
        headerFile += `,\n`;    
    }
    headerFile += `\t` + element.toUpperCase();
    bFirstEnum = false;
});
headerFile += `\n} E${prefix}ObjectType;\n\n`;

headerFile += `
#define NUM_GTMX_${prefix}_OBJECTS ${tmx_objects_count}

extern TMX_Object GTMX_${prefix}Objects[NUM_GTMX_${prefix}_OBJECTS];

#endif
`;

let codeFile = `#include "${only_filename_out_header}"

TMX_Object GTMX_${prefix}Objects[NUM_GTMX_${prefix}_OBJECTS] = {
`;

let bFirstObject = true;
function parserObj(obj) {
    if(obj.name && obj.type) {
        if(obj.type == "element" && obj.name == "object" && obj.attributes)
        {
            let typeID = obj.attributes.type;
            let uniqueID = undefined;
            if(obj.children && Array.isArray(obj.children)) {
                obj.children.forEach(element => {
                    if(element.type == "element" && element.name == "properties" && element.children && Array.isArray(element.children))
                    {
                        element.children.forEach(element2 => {
                            if(element2.type == "element" && element2.name == "property" && element2.attributes)
                            {
                                if(element2.attributes.name == "uniqueID") {
                                    uniqueID = element2.attributes.value;
                                }
                            }
                        });
                    }
                });
            }

            if(obj.attributes.x && obj.attributes.x && typeID != undefined && uniqueID != undefined)
            {
                typeID = typeID.toUpperCase();
                const x = parseInt(obj.attributes.x);
                const y = parseInt(obj.attributes.y);
                if(!bFirstObject)
                {
                    codeFile += `,`;
                }
                if(obj.attributes.width && obj.attributes.height)
                {
                    const width = parseInt(obj.attributes.width);
                    const height = parseInt(obj.attributes.height);
                    const maxX = x + width;
                    const maxY = y + height;
                    codeFile += ` {${typeID}, ${uniqueID}, {{${x}, ${y}}, {${maxX}, ${maxY}}}}`;
                    // console.log(`Rectangle: ${uniqueID} typeID: ${typeID} X: ${obj.attributes.x} Y: ${obj.attributes.y} W: ${obj.attributes.width} H: ${obj.attributes.height}`);
                } else {
                    codeFile += ` {${typeID}, ${uniqueID}, {{${x}, ${y}}, {0, 0}}}`;
                    // console.log(`Rectangle: ${uniqueID} typeID: ${typeID} X: ${obj.attributes.x} Y: ${obj.attributes.y}`);
                }
                bFirstObject = false;
            }
            // console.log(obj.name);
        }
    }

    if(obj.children && Array.isArray(obj.children)) {
        obj.children.forEach(element => {
            parserObj(element);
        });
    }
}

parserObj(jObj2);

codeFile += `\n};\n`;
// console.log(headerFile);

fs.writeFileSync(filename_out_h, headerFile);
fs.writeFileSync(filename_out_c, codeFile);
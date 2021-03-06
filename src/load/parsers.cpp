#include "parsers.h"

#include <QFile>
#include <QTextStream>

#include "gcode/gcode.h"

bool ExampleGCodeParser::loadToolpath(const QFileInfo &info,
                                      Toolpath &path) const
{
    // this is mostly pulled from Matt Metts' original toolpathviz gcode parser

    if(not info.exists())
        return false;

    ifstream file;

    file.open(info.canonicalFilePath().toStdString().c_str());

    uint linenum = 0;
    qreal x, y, z;

    while (file.good()) {
        string line;

        std::getline(file, line);
        gcode code = gcode(line);

        if (code.hasCode('G') && (int)code.getCodeValue('G') == 1) {
            if (code.hasCode('X')) {
                x = code.getCodeValue('X');
            }
            if (code.hasCode('Y')) {
               y = code.getCodeValue('Y');
            }
            if (code.hasCode('Z')) {
                z = code.getCodeValue('Z');
            }
            path.append(new MoveAbsoluteStep(linenum, x, y, z, 0, 0));
        }

        linenum++;
    }
    return true;
}

bool FullExampleGCodeParser::loadToolpath(const QFileInfo &info,
                                          Toolpath &path) const
{
    // this is mostly pulled from Matt Metts' original toolpathviz gcode parser

    if(not info.exists())
        return false;

    std::ifstream file;

    file.open(info.canonicalFilePath().toStdString().c_str());

    uint linenum = 0;
    qreal x, y, z, a, b;
    int toolhead = 0;

    while (file.good()) {
        string line;

        std::getline(file, line);
        gcode code = gcode(line);

        if(code.hasCode('F'))
            path.append(new SetFeedrateStep(linenum, code.getCodeValue('F')));

        if(code.hasCode('T') && code.getCodeValue('T') != toolhead) {
            toolhead = code.getCodeValue('T');
            path.append(new SetToolheadStep(linenum, toolhead));
        }

        if (code.hasCode('G') && (int)code.getCodeValue('G') == 1) {
            if (code.hasCode('X')) {
                x = code.getCodeValue('X');
            }
            if (code.hasCode('Y')) {
                y = code.getCodeValue('Y');
            }
            if (code.hasCode('Z')) {
                if(code.getCodeValue('Z') != z)
                    path.append(new NewLayerStep(linenum, z));
                z = code.getCodeValue('Z');
            }
            if (code.hasCode('A')) {
                a = code.getCodeValue('A');
            }
            if (code.hasCode('B')) {
                b = code.getCodeValue('B');
            }
            if (code.hasCode('E')) {
                if(toolhead == 0)
                    a = code.getCodeValue('E');
                else if(toolhead == 1)
                    b = code.getCodeValue('E');
            }
            path.append(new MoveAbsoluteStep(linenum, x, y, z, a, b));
        }

        linenum++;
    }
    return true;
}

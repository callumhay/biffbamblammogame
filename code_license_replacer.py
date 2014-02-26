
import os
import re
import glob

allFileNames  = glob.glob("*.h") + glob.glob("*.cpp")
allFileNames += glob.glob("BlammoEngine/*.h") + glob.glob("BlammoEngine/*.cpp")
allFileNames += glob.glob("ESPEngine/*.h") + glob.glob("ESPEngine/*.cpp")
allFileNames += glob.glob("GameModel/*.h") + glob.glob("GameModel/*.cpp")
allFileNames += glob.glob("GameView/*.h") + glob.glob("GameView/*.cpp")
allFileNames += glob.glob("GameControl/*.h") + glob.glob("GameControl/*.cpp")
allFileNames += glob.glob("GameSound/*.h") + glob.glob("GameSound/*.cpp")

# Get the license Info...
licenseTextLines = None
with open("./code_license.txt") as licenseFile:
	licenseTextLines = licenseFile.readlines()

if licenseTextLines is None:
	print "Failed to find and open code_license.txt in script directory."
	exit(-1)

# Clean up the license text in preparation for placing it into the code as a comment...
licenseText = ""
for licenseTextLine in licenseTextLines:
	licenseText += " * " + licenseTextLine.strip() + "\n"


for fileName in allFileNames:
	with open(fileName, 'r+') as file:
		fileText = file.read()
		modifiedFileText = re.sub(r"^\W*/\*\*(.+?)\*/", "/**\n * " + os.path.basename(fileName) + "\n * \n" + licenseText + " */", fileText, count=1, flags=re.DOTALL)
		
		file.seek(0)
		file.write(modifiedFileText)
		file.truncate()
		file.close()

		

package bbbleveleditor;

import java.io.File;

import javax.swing.filechooser.FileFilter;

public class BBBLevelFileFilter extends FileFilter {
	public static final String BBB_LEVEL_FILE_EXT = "lvl";
	
	@Override
	public boolean accept(File f) {
		if (f.isDirectory()) {
			return true;
		}
		
		String fileExtension = BBBLevelFileFilter.getExtension(f);
		if (fileExtension != null) {
			if (fileExtension.equals(BBB_LEVEL_FILE_EXT)) {
				return true;
			}
		}
		
		return false;
	}

	@Override
	public String getDescription() {
		return "Biff! Bam!! Blammo!?! Level Files (*.lvl)";
	}
	public static String changeExtensionToBBBLvlExt(String fileName) {
		String result = fileName;
		if (BBBLevelFileFilter.getExtension(result) != BBBLevelFileFilter.BBB_LEVEL_FILE_EXT) {
			int dotPos = result.lastIndexOf('.');
			if (dotPos == -1) {
				dotPos = result.length();
			}
			String fileNameNoExt = result.substring(0, dotPos);
			result = fileNameNoExt + "." + BBBLevelFileFilter.BBB_LEVEL_FILE_EXT;
		}
		return result;
	}
    /*
     * Get the extension of a file.
     */  
    public static String getExtension(File f) {
    	return getExtension(f.getName());
    }
    
    public static String getExtension(String s) {
        String ext = null;
        int i = s.lastIndexOf('.');

        if (i > 0 &&  i < s.length() - 1) {
            ext = s.substring(i+1).toLowerCase();
        }
        return ext;
    } 
}

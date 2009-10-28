package bbbleveleditor;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Scanner;

import javax.swing.JOptionPane;

public class ItemDrop {
	public static ArrayList<ItemDrop> powerUpItems = new ArrayList<ItemDrop>();
	public static ArrayList<ItemDrop> powerNeutralItems = new ArrayList<ItemDrop>();;
	public static ArrayList<ItemDrop> powerDownItems = new ArrayList<ItemDrop>();;
	
	public enum ItemType { POWER_UP, POWER_NEUTRAL, POWER_DOWN };
	
	public static final int NO_LIKELIHOOD	  = 0;
	public static final int LOW_LIKELIHOOD 	  = 1;
	public static final int MEDIUM_LIKELIHOOD = 2;
	public static final int HIGH_LIKELIHOOD   = 3;
	public static final String[] LIKEYIHOOD_STRINGS = { "None", "Low", "Medium", "High" };
	
	private String name;
	private String description;
	private ItemType type;
	
	private ItemDrop(String name, String desc, ItemType type) {
		this.name = name;
		this.description = desc;
		this.type = type;
	}
	
	public String getName() {
		return this.name;
	}
	public String getDescription() {
		return this.description;
	}
	public ItemType getType() {
		return this.type;
	}
	
	public static void readItemTypesFile() {
		try {
			InputStream itemTypeFileStream = BBBLevelEditorMain.class.getResourceAsStream("resources/bbb_item_types.txt");
			Scanner itemFileScanner = new Scanner(itemTypeFileStream);
			
			String itemDesc = "";
			String itemName = "";
			String itemTypeStr = "";
			ItemType itemType;
			
			try {
				while (itemFileScanner.hasNext()) {
					itemTypeStr = itemFileScanner.next();
					if (itemTypeStr.isEmpty()) {
						continue;
					}
					
					ArrayList<ItemDrop> tempItemList = null;
					
					// Assign the type based on what was read in
					if (itemTypeStr.equals("+")) {
						// Power-up item type
						itemType = ItemType.POWER_UP;
						tempItemList = ItemDrop.powerUpItems;
					}
					else if (itemTypeStr.equals("-")) {
						// Power-down item type
						itemType = ItemType.POWER_DOWN;
						tempItemList = ItemDrop.powerDownItems;
					}
					else if (itemTypeStr.equals("~")) {
						// Neutral item type
						itemType = ItemType.POWER_NEUTRAL;
						tempItemList = ItemDrop.powerNeutralItems;
					}
					else {
						assert(false);
						JOptionPane.showMessageDialog(null, 
								"Invalid format found in the level item definitions file (resources/bbb_item_types.txt)!\n",
								"Error", JOptionPane.ERROR_MESSAGE);
						powerUpItems.clear();
						powerNeutralItems.clear();
						powerDownItems.clear();
						return;
					}
					assert(tempItemList != null);
					
					// Read in the name of the item
					itemName = itemFileScanner.nextLine().trim();
					// Read in the description of the item
					itemDesc = itemFileScanner.nextLine().trim();
					
					tempItemList.add(new ItemDrop(itemName, itemDesc, itemType));
				}
			}
			catch (Exception e) { 
				powerUpItems.clear();
				powerNeutralItems.clear();
				powerDownItems.clear();

				JOptionPane.showMessageDialog(null, 
						"Invalid format found in the level item definitions file (resources/bbb_item_types.txt)!\n",
						"Error", JOptionPane.ERROR_MESSAGE);
				
			}
			finally { 
				itemFileScanner.close(); 
			}

		}
		catch (Exception e) {
			powerUpItems.clear();
			powerNeutralItems.clear();
			powerDownItems.clear();
			JOptionPane.showMessageDialog(null, 
					"Could not open the level item definitions file (resources/bbb_item_types.txt)!", 
					"Error", JOptionPane.ERROR_MESSAGE);
		}
	}
	
	public static HashMap<String, Integer> populateDefaultItemDropSettings() {
		int totalSize = ItemDrop.powerUpItems.size() + ItemDrop.powerNeutralItems.size() + ItemDrop.powerDownItems.size();
		HashMap<String, Integer> settings = new HashMap<String, Integer>(totalSize);
		
		ItemDrop temp = null;
		for (int i = 0; i < ItemDrop.powerUpItems.size(); i++) {
			temp = ItemDrop.powerUpItems.get(i);
			settings.put(temp.getName(), ItemDrop.MEDIUM_LIKELIHOOD);
		}
		for (int i = 0; i < ItemDrop.powerNeutralItems.size(); i++) {
			temp = ItemDrop.powerNeutralItems.get(i);
			settings.put(temp.getName(), ItemDrop.MEDIUM_LIKELIHOOD);
		}
		for (int i = 0; i < ItemDrop.powerDownItems.size(); i++) {
			temp = ItemDrop.powerDownItems.get(i);
			settings.put(temp.getName(), ItemDrop.MEDIUM_LIKELIHOOD);
		}	
		return settings;
	}	
}

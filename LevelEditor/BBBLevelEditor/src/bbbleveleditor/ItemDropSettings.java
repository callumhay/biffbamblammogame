package bbbleveleditor;

public class ItemDropSettings {
	
	private int probabilityValue;
	private boolean onlyDropIfUnlocked;

	public ItemDropSettings(int probability, boolean onlyDropIfUnlocked) {
		this.probabilityValue = probability;
		this.onlyDropIfUnlocked = onlyDropIfUnlocked;
	}
	
	public int GetProbabilityValue() {
		return this.probabilityValue;
	}
	
	public boolean GetOnlyDropIfUnlocked() {
		return this.onlyDropIfUnlocked;
	}
	
	public void IncrementProbability() {
		this.probabilityValue++;
	}
}

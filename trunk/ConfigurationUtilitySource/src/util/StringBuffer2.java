package util;


public class StringBuffer2 {
	
	private StringBuffer strBuff;
	
	public StringBuffer2() {
		strBuff = new StringBuffer();
	}
	
	public void appendN(String str) {
		appendT(str,0);
	}
	
	public void appendT(String str, int numT) {
		for(int x=0; x < numT; x++)
			strBuff.append("\t");
		strBuff.append(str);
		strBuff.append("\n");
	}
	
	public void append(String str) {
		strBuff.append(str);
	}
	
	public void append(Object o) {
		strBuff.append(o);
	}
	
	@Override
	public String toString() {
		return strBuff.toString();
	}
}

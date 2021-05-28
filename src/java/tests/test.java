public class Test {
  public static void main(String[] args) {
    String sb = new String("abc");
    String anotherSb = "abc";
    if (sb.equals(anotherSb)) System.out.println("sb");
    else {
      return 1;
    }
    outputTextArea.setText("i\tm(i)\n");
    double res = 0.0;
    for(int i = 1; i <= 901; ++i) {
        if(i % 2 == 1) res += 1.0 / (2 * i - 1);
        else res -= 1.0 / (2 * i - 1);
        if((i - 1) % 100 == 0) {
            outputTextArea.append(new DecimalFormat("0.0000").format(4 * res));
        }
    }
    JOptionPane.showMessageDialog(null, outputTextArea, "Estimate PI", JOptionPane.INFORMATION_MESSAGE);
  }
}
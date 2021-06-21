import java.text.DecimalFormat;
import javax.swing.JOptionPane;
import javax.swing.JTextArea;

public class EstimatePI {
    public static void main(String[] args) {
        JTextArea outputTextArea = new JTextArea();
        outputTextArea.setText("i\tm(i)\n");
        double res = 0.0;
        for(int i = 1; i <= 901; ++i) {
            if(i % 2 == 1) res += 1.0 / (2 * i - 1);
            else res -= 1.0 / (2 * i - 1);
            if((i - 1) % 100 == 0) {
                outputTextArea.append(i + "\t" + new DecimalFormat("0.0000").format(4 * res) + "\n");
            }
        }
        JOptionPane.showMessageDialog(null, outputTextArea, "Estimate PI", JOptionPane.INFORMATION_MESSAGE);
    }
}

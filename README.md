# Lie Detector

**Technologies/Components:** Arduino UNO, Pulse Sensor, Humidity Sensor, LCD, LED

**Description:**  
Hardware device that measures skin moisture and heart rate to detect stress and possible lies.

**How it works:**  
- Measures humidity and pulse every second for 15 seconds.  
- Calculates the average values.  
- If the averages are below predefined thresholds:
  - The LCD displays "You lied"  
  - The LED turns red  
- If the averages are above thresholds:
  - The LCD displays "You are telling the truth"  
  - The LED turns green

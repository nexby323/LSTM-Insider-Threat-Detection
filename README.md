# Insider Threat Detection using LSTM 🛡️

## מטרה (Goal)
פרויקט זה נועד לפתח מערכת לזיהוי חריגות (Anomaly Detection) מבוססת רשתות Long Short-Term Memory (LSTM), במטרה לזהות איומי פנים (Insider Threats) במערכות מחשוב. המודל מתבסס על ארכיטקטורת LADOHD (LSTM-based Anomaly Detector Over High-dimensional Data). המטרה היא ללמוד תבניות התנהגות נורמליות של מערכת מתוך רצפי אירועים (System Events), ולזהות רצפים זדוניים וחריגים המוסווים בתוך פעולות לגיטימיות, תוך פתרון בעיית ה-Order-Aware Recognition (OAR) ברצפים ארוכים.

## ארכיטקטורת המערכת (System Architecture)
המערכת מורכבת מארבעה שלבים עיקריים:
1. **Data Generation:** איסוף רצפי פעולות (Events) מתחנות קצה (EDR).
2. **Data Selection:** סינון מידע רלוונטי לאפליקציות ספציפיות והמרתו לוקטורים קטגוריאליים (Vocabulary of events).
3. **Model Generation:** אימון מודל LSTM על רצפים של התנהגות תקינה בלבד (Benign data).
4. **Anomaly Detector:** חיזוי ההסתברות של כל אירוע חדש, וסיווג אירועים בעלי הסתברות נמוכה באופן דינמי כחריגים (Anomalous).

## מפרט טכני של המודל (Technical Details)
- **קידוד (Embedding):** גודל 16.
- **שכבות (Layers):** 3 שכבות LSTM רצופות, ולאחריהן שכבה ליניארית (Linear/Dense) עם 100 נוירונים.
- **אימון:** Batch size של 64, חלון זמן (BPTT) של 64, ושימוש באלגוריתמי אופטימיזציה כמו SGDR ו-Cyclical Learning Rates.
- **אסטרטגיית זיהוי:** בניית קבוצה $K$ דינמית של האירועים הסבירים ביותר, וסיווג האירוע הנוכחי בהתאם להימצאותו בקבוצה.

---
**Author:** מעיין אושרי
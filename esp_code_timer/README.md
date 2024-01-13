# ESP Code timer

Records a timestamp in us and displays the result.

## Simple example
```
for loop {
    esp_code_timer_start();
    <code block to measure>
    uint32_t time = esp_code_timer_stop();
}

uint32_t avg = esp_code_timer_get_average();

```

## Example
```
esp_code_timer_t timer;
esp_code_timer_init(&timer, "dcd timing", 1000, /*when to display*/1000);


esp_code_timer_take_timestamp(&timer,"_dcd top");
(...)
esp_code_timer_take_timestamp(&timer,"_dcd end");
(...)



-------------------------------------------------
dcd timing
-------------------------------------------------
I (8354) : 726: _dcd top          939841        4152
I (8364) : 727: _dcd end          939847        6
I (8374) : 728: _dcd top          939872        25
I (8384) : 729: _dcd end          939880        8
I (8394) : 730: _dcd top          941969        2089
I (8404) : 731: _dcd end          941975        6
I (8414) : 732: _dcd top          941992        17
I (8424) : 733: _dcd end          942000        8
I (8434) : 734: _dcd top          943969        1969


```
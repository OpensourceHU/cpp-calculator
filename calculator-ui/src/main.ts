import { invoke } from "@tauri-apps/api/core";

interface HistoryItem {
  expression: string;
  result: number;
}

let history: HistoryItem[] = JSON.parse(localStorage.getItem("calc_history") || "[]");

window.addEventListener("DOMContentLoaded", () => {
  const formulaDisplay = document.getElementById("formula-display") as HTMLDivElement;
  const valueDisplay = document.getElementById("value-display") as HTMLDivElement;
  const statusIndicator = document.getElementById("status-indicator") as HTMLSpanElement;
  const expressionInput = document.getElementById("expression-input") as HTMLInputElement;
  const historyList = document.getElementById("history-list") as HTMLDivElement;
  const historyEmpty = document.getElementById("history-empty") as HTMLDivElement;
  const clearHistoryBtn = document.getElementById("clear-history-btn") as HTMLButtonElement;

  function updateHistoryUI() {
    // Remove existing items from display
    const items = historyList.querySelectorAll(".history-item");
    items.forEach(el => el.remove());

    if (history.length === 0) {
      historyEmpty.style.display = "block";
      return;
    }
    historyEmpty.style.display = "none";
    
    // Add items (reverse order to show newest first)
    history.slice().reverse().forEach((item) => {
      const itemEl = document.createElement("div");
      itemEl.className = "history-item";
      itemEl.innerHTML = `
        <div class="history-expr">${item.expression}</div>
        <div class="history-result">${item.result}</div>
      `;
      itemEl.addEventListener("click", () => {
        expressionInput.value = item.expression;
        valueDisplay.textContent = item.result.toString();
        formulaDisplay.textContent = `${item.expression} =`;
        showStatus("OK", false);
        expressionInput.focus();
      });
      historyList.appendChild(itemEl);
    });
  }

  function showStatus(text: string, isError: boolean) {
    statusIndicator.textContent = text;
    statusIndicator.className = "display-indicator visible";
    if (isError) {
      statusIndicator.classList.add("error");
    }
  }

  function hideStatus() {
    statusIndicator.className = "display-indicator";
  }

  // Perform official evaluation (add to history)
  async function performCalculation() {
    const expr = expressionInput.value.trim();
    if (!expr) return;

    try {
      showStatus("EVAL", false);
      const jsonResponse: string = await invoke("evaluate_expression", { expression: expr });
      const data = JSON.parse(jsonResponse);

      if (data.status === "success") {
        const result = data.result;
        valueDisplay.textContent = result.toString();
        formulaDisplay.textContent = `${expr} =`;
        showStatus("SUCCESS", false);

        // Add to history list if it's new
        const isDuplicate = history.length > 0 && history[history.length - 1].expression === expr;
        if (!isDuplicate) {
          history.push({ expression: expr, result });
          if (history.length > 50) history.shift();
          localStorage.setItem("calc_history", JSON.stringify(history));
          updateHistoryUI();
        }
      } else {
        valueDisplay.textContent = "Error";
        formulaDisplay.textContent = data.message;
        showStatus("ERROR", true);
      }
    } catch (err) {
      valueDisplay.textContent = "Error";
      formulaDisplay.textContent = String(err);
      showStatus("ERR_COMM", true);
    }
  }

  // Live evaluation while typing (Debounced)
  let debounceTimeout: number;
  expressionInput.addEventListener("input", () => {
    clearTimeout(debounceTimeout);
    debounceTimeout = setTimeout(async () => {
      const expr = expressionInput.value.trim();
      if (!expr) {
        valueDisplay.textContent = "0";
        formulaDisplay.textContent = "";
        hideStatus();
        return;
      }
      try {
        const jsonResponse: string = await invoke("evaluate_expression", { expression: expr });
        const data = JSON.parse(jsonResponse);
        if (data.status === "success") {
          valueDisplay.textContent = data.result.toString();
          showStatus("OK", false);
        } else {
          // Don't show flashing errors while typing
          hideStatus();
        }
      } catch (err) {
        hideStatus();
      }
    }, 150) as unknown as number;
  });

  // Handle keypad button clicks
  document.querySelectorAll(".key").forEach((btn) => {
    btn.addEventListener("click", () => {
      const action = btn.getAttribute("data-action");
      const val = btn.getAttribute("data-val");

      if (action === "all-clear") {
        expressionInput.value = "";
        valueDisplay.textContent = "0";
        formulaDisplay.textContent = "";
        hideStatus();
        expressionInput.focus();
      } else if (action === "delete") {
        const start = expressionInput.selectionStart ?? expressionInput.value.length;
        const end = expressionInput.selectionEnd ?? expressionInput.value.length;
        
        if (start !== end) {
          expressionInput.value = 
            expressionInput.value.substring(0, start) + 
            expressionInput.value.substring(end);
          expressionInput.setSelectionRange(start, start);
        } else if (start > 0) {
          expressionInput.value = 
            expressionInput.value.substring(0, start - 1) + 
            expressionInput.value.substring(start);
          expressionInput.setSelectionRange(start - 1, start - 1);
        }
        expressionInput.dispatchEvent(new Event("input"));
        expressionInput.focus();
      } else if (action === "calculate") {
        performCalculation();
      } else if (val) {
        // Append value at cursor position
        const start = expressionInput.selectionStart ?? expressionInput.value.length;
        const end = expressionInput.selectionEnd ?? expressionInput.value.length;
        
        expressionInput.value = 
          expressionInput.value.substring(0, start) + 
          val + 
          expressionInput.value.substring(end);
        
        const newCursorPos = start + val.length;
        expressionInput.setSelectionRange(newCursorPos, newCursorPos);
        expressionInput.dispatchEvent(new Event("input"));
        expressionInput.focus();
      }
    });
  });

  // Handle Enter key inside the text input
  expressionInput.addEventListener("keydown", (e) => {
    if (e.key === "Enter") {
      e.preventDefault();
      performCalculation();
    }
  });

  // Handle Clear History button
  clearHistoryBtn.addEventListener("click", () => {
    history = [];
    localStorage.removeItem("calc_history");
    updateHistoryUI();
  });

  // Initialize UI state
  updateHistoryUI();
});

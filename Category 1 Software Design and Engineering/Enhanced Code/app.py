"""
app.py - Main entry point for the Animal Shelter dashboard.
"""

from dash import Dash
from data import AnimalShelter
from layout import create_layout
from callbacks import register_callbacks

# Initialize database and app
shelter = AnimalShelter()
app = Dash(__name__)
app.layout = create_layout()

# Register callbacks
register_callbacks(app, shelter)

if __name__ == "__main__":
    app.run_server(debug=True)

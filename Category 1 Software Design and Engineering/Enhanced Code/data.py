"""
data.py - Handles all database interactions for the Animal Shelter app.
"""

import os
from pymongo import MongoClient
from dotenv import load_dotenv

load_dotenv()  # Load credentials from .env file

class AnimalShelter:
    """CRUD operations for Animal collection in MongoDB."""

    def __init__(self):
        # Secure: credentials from environment variables
        USER = os.getenv("MONGO_USER")
        PASS = os.getenv("MONGO_PASS")
        HOST = os.getenv("MONGO_HOST", "localhost")
        PORT = os.getenv("MONGO_PORT", "27017")
        DB = os.getenv("MONGO_DB", "AAC")
        COL = os.getenv("MONGO_COL", "animals")

        if not USER or not PASS:
            raise ValueError("Missing database credentials in environment variables.")

        self.client = MongoClient(f"mongodb://{USER}:{PASS}@{HOST}:{PORT}/{DB}")
        self.database = self.client[DB]
        self.collection = self.database[COL]

    def create(self, data: dict) -> bool:
        """Insert a new record after validation."""
        if not data or not isinstance(data, dict):
            raise ValueError("Invalid data: must be a non-empty dictionary.")
        result = self.collection.insert_one(data)
        return bool(result.inserted_id)

    def read(self, query: dict = None):
        """Read records. Returns list instead of raw cursor for easier reuse."""
        if query is None:
            query = {}
        docs = list(self.collection.find(query))
        return docs

    def update(self, query: dict, new_values: dict) -> int:
        """Update records matching query. Returns count of updated docs."""
        if not query or not new_values:
            raise ValueError("Query and update values must be provided.")
        result = self.collection.update_many(query, {"$set": new_values})
        return result.modified_count

    def delete(self, query: dict) -> int:
        """Delete records matching query. Returns count of deleted docs."""
        if not query:
            raise ValueError("Query must be provided for deletion.")
        result = self.collection.delete_many(query)
        return result.deleted_count
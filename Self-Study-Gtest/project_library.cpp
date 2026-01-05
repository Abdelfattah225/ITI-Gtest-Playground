#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <map>

// ============================================================
// INTERFACES
// ============================================================

class INotificationService
{
public:
    virtual ~INotificationService() = default;
    virtual void sendNotification(const std::string &memberId,
                                  const std::string &message) = 0;
};

// ============================================================
// DOMAIN CLASSES
// ============================================================

class Book
{
private:
    std::string id;
    std::string title;
    std::string author;
    bool available;

public:
    Book(const std::string &id, const std::string &title, const std::string &author)
        : id(id), title(title), author(author), available(true) {}

    std::string getId() const { return id; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    bool isAvailable() const { return available; }

    void markBorrowed() { available = false; }
    void markReturned() { available = true; }
};

class Member
{
private:
    std::string id;
    std::string name;
    std::vector<std::string> borrowedBookIds;
    int maxBooks;

public:
    Member(const std::string &id, const std::string &name, int maxBooks = 3)
        : id(id), name(name), maxBooks(maxBooks) {}

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    int getBorrowedCount() const { return borrowedBookIds.size(); }
    int getMaxBooks() const { return maxBooks; }

    bool canBorrow() const { return borrowedBookIds.size() < maxBooks; }

    void addBorrowedBook(const std::string &bookId)
    {
        borrowedBookIds.push_back(bookId);
    }

    bool removeBorrowedBook(const std::string &bookId)
    {
        for (auto it = borrowedBookIds.begin(); it != borrowedBookIds.end(); ++it)
        {
            if (*it == bookId)
            {
                borrowedBookIds.erase(it);
                return true;
            }
        }
        return false;
    }

    bool hasBorrowedBook(const std::string &bookId) const
    {
        for (const auto &id : borrowedBookIds)
        {
            if (id == bookId)
                return true;
        }
        return false;
    }
};

// ============================================================
// LIBRARY CLASS (Main class under test)
// ============================================================

class Library
{
private:
    std::map<std::string, Book *> books;
    std::map<std::string, Member *> members;
    INotificationService *notificationService;

public:
    Library(INotificationService *notifService)
        : notificationService(notifService) {}

    ~Library()
    {
        for (auto &pair : books)
            delete pair.second;
        for (auto &pair : members)
            delete pair.second;
    }

    // Book management
    void addBook(const std::string &id, const std::string &title, const std::string &author)
    {
        books[id] = new Book(id, title, author);
    }

    Book *findBook(const std::string &id)
    {
        auto it = books.find(id);
        return (it != books.end()) ? it->second : nullptr;
    }

    int getAvailableBookCount()
    {
        int count = 0;
        for (const auto &pair : books)
        {
            if (pair.second->isAvailable())
                count++;
        }
        return count;
    }

    // Member management
    void registerMember(const std::string &id, const std::string &name)
    {
        members[id] = new Member(id, name);
    }

    Member *findMember(const std::string &id)
    {
        auto it = members.find(id);
        return (it != members.end()) ? it->second : nullptr;
    }

    // Core functionality: Borrow a book
    bool borrowBook(const std::string &memberId, const std::string &bookId)
    {
        Member *member = findMember(memberId);
        Book *book = findBook(bookId);

        // Validation
        if (!member || !book)
            return false;
        if (!book->isAvailable())
            return false;
        if (!member->canBorrow())
            return false;

        // Perform borrow
        book->markBorrowed();
        member->addBorrowedBook(bookId);

        // Send notification
        std::string message = "You have borrowed: " + book->getTitle();
        notificationService->sendNotification(memberId, message);

        return true;
    }

    // Core functionality: Return a book
    bool returnBook(const std::string &memberId, const std::string &bookId)
    {
        Member *member = findMember(memberId);
        Book *book = findBook(bookId);

        // Validation
        if (!member || !book)
            return false;
        if (!member->hasBorrowedBook(bookId))
            return false;

        // Perform return
        book->markReturned();
        member->removeBorrowedBook(bookId);

        // Send notification
        std::string message = "You have returned: " + book->getTitle();
        notificationService->sendNotification(memberId, message);

        return true;
    }
};

// ============================================================
// FAKE NOTIFICATION SERVICE (for testing)
// ============================================================

class FakeNotificationService : public INotificationService
{
private:
    std::vector<std::pair<std::string, std::string>> sentNotifications;

public:
    void sendNotification(const std::string &memberId,
                          const std::string &message) override
    {
        sentNotifications.push_back({memberId, message});
    }

    int getNotificationCount() const
    {
        return sentNotifications.size();
    }

    std::string getLastMessage() const
    {
        if (sentNotifications.empty())
            return "";
        return sentNotifications.back().second;
    }

    std::string getLastRecipient() const
    {
        if (sentNotifications.empty())
            return "";
        return sentNotifications.back().first;
    }

    void clear()
    {
        sentNotifications.clear();
    }
};

// ============================================================
// TEST FIXTURE
// ============================================================

class LibraryTest : public ::testing::Test
{
protected:
    FakeNotificationService fakeNotifier;
    Library *library;

    void SetUp() override
    {
        library = new Library(&fakeNotifier);

        // Add some default test data
        library->addBook("B001", "Clean Code", "Robert Martin");
        library->addBook("B002", "Design Patterns", "Gang of Four");
        library->addBook("B003", "The Pragmatic Programmer", "Hunt & Thomas");

        library->registerMember("M001", "Alice");
        library->registerMember("M002", "Bob");
    }

    void TearDown() override
    {
        delete library;
    }
};

// ============================================================
// TESTS: Book Management
// ============================================================

TEST_F(LibraryTest, AddBookIncreasesCount)
{
    int initialCount = library->getAvailableBookCount();
    library->addBook("B004", "New Book", "Author");
    EXPECT_EQ(library->getAvailableBookCount(), initialCount + 1);
}

TEST_F(LibraryTest, FindBookReturnsCorrectBook)
{
    Book *book = library->findBook("B001");
    ASSERT_NE(book, nullptr);
    EXPECT_EQ(book->getTitle(), "Clean Code");
    EXPECT_EQ(book->getAuthor(), "Robert Martin");
}

TEST_F(LibraryTest, FindNonExistentBookReturnsNull)
{
    Book *book = library->findBook("INVALID");
    EXPECT_EQ(book, nullptr);
}

// ============================================================
// TESTS: Member Management
// ============================================================

TEST_F(LibraryTest, RegisteredMemberCanBeFound)
{
    Member *member = library->findMember("M001");
    ASSERT_NE(member, nullptr);
    EXPECT_EQ(member->getName(), "Alice");
}

TEST_F(LibraryTest, FindNonExistentMemberReturnsNull)
{
    Member *member = library->findMember("INVALID");
    EXPECT_EQ(member, nullptr);
}

// ============================================================
// TESTS: Borrowing Books
// ============================================================

TEST_F(LibraryTest, BorrowBookSucceeds)
{
    bool result = library->borrowBook("M001", "B001");
    EXPECT_TRUE(result);
}

TEST_F(LibraryTest, BorrowBookMarksBookUnavailable)
{
    library->borrowBook("M001", "B001");
    Book *book = library->findBook("B001");
    EXPECT_FALSE(book->isAvailable());
}

TEST_F(LibraryTest, BorrowBookUpdatesMemberCount)
{
    Member *member = library->findMember("M001");
    int initialCount = member->getBorrowedCount();

    library->borrowBook("M001", "B001");

    EXPECT_EQ(member->getBorrowedCount(), initialCount + 1);
}

TEST_F(LibraryTest, BorrowBookSendsNotification)
{
    library->borrowBook("M001", "B001");

    EXPECT_EQ(fakeNotifier.getNotificationCount(), 1);
    EXPECT_EQ(fakeNotifier.getLastRecipient(), "M001");
    EXPECT_EQ(fakeNotifier.getLastMessage(), "You have borrowed: Clean Code");
}

TEST_F(LibraryTest, CannotBorrowUnavailableBook)
{
    library->borrowBook("M001", "B001");               // First borrow succeeds
    bool result = library->borrowBook("M002", "B001"); // Second should fail
    EXPECT_FALSE(result);
}

TEST_F(LibraryTest, CannotBorrowWithInvalidMember)
{
    bool result = library->borrowBook("INVALID", "B001");
    EXPECT_FALSE(result);
}

TEST_F(LibraryTest, CannotBorrowInvalidBook)
{
    bool result = library->borrowBook("M001", "INVALID");
    EXPECT_FALSE(result);
}

// ============================================================
// TESTS: Returning Books
// ============================================================

TEST_F(LibraryTest, ReturnBookSucceeds)
{
    library->borrowBook("M001", "B001");
    bool result = library->returnBook("M001", "B001");
    EXPECT_TRUE(result);
}

TEST_F(LibraryTest, ReturnBookMarksBookAvailable)
{
    library->borrowBook("M001", "B001");
    library->returnBook("M001", "B001");

    Book *book = library->findBook("B001");
    EXPECT_TRUE(book->isAvailable());
}

TEST_F(LibraryTest, ReturnBookUpdatesMemberCount)
{
    library->borrowBook("M001", "B001");
    Member *member = library->findMember("M001");
    int countAfterBorrow = member->getBorrowedCount();

    library->returnBook("M001", "B001");

    EXPECT_EQ(member->getBorrowedCount(), countAfterBorrow - 1);
}

TEST_F(LibraryTest, ReturnBookSendsNotification)
{
    library->borrowBook("M001", "B001");
    fakeNotifier.clear(); // Clear the borrow notification

    library->returnBook("M001", "B001");

    EXPECT_EQ(fakeNotifier.getNotificationCount(), 1);
    EXPECT_EQ(fakeNotifier.getLastMessage(), "You have returned: Clean Code");
}

TEST_F(LibraryTest, CannotReturnBookNotBorrowed)
{
    bool result = library->returnBook("M001", "B001"); // Never borrowed
    EXPECT_FALSE(result);
}

TEST_F(LibraryTest, CannotReturnBookBorrowedByOtherMember)
{
    library->borrowBook("M001", "B001");               // Alice borrows
    bool result = library->returnBook("M002", "B001"); // Bob tries to return
    EXPECT_FALSE(result);
}

// ============================================================
// TASK Add these 3 new tests to the file:
// ============================================================

/* Test 1: CannotBorrowMoreThanMaxBooks

    Member can borrow max 3 books (default limit)
    Make Alice borrow 3 books (B001, B002, B003)
    Add a 4th book: library->addBook("B004", "Fourth Book", "Author")
    Try to borrow B004 - should fail
*/

TEST_F(LibraryTest, CannotBorrowMoreThanMaxBooks)
{
    library->borrowBook("M001", "B001"); // Alice borrows
    library->borrowBook("M001", "B00٢"); // Alice borrows
    library->borrowBook("M001", "B00٣"); // Alice borrows

    library->addBook("B004", "Fourth Book", "Author");

    bool result = library->borrowBook("M001", "B001"); // Alice borrows

    EXPECT_FALSE(result);
}

/*

Test 2: MemberCanBorrowMultipleBooks

    Make Alice borrow B001 and B002
    Check that her borrowed count is 2
    Check both books are unavailable

*/

TEST_F(LibraryTest, MemberCanBorrowMultipleBooks)
{
    library->borrowBook("M001", "B001"); // Alice borrows
    library->borrowBook("M001", "B002"); // Alice borrows

    Member *Mem = library->findMember("M001");
    int borrowedcount = Mem->getBorrowedCount();
    EXPECT_EQ(borrowedcount, 2);

    Book *boo1 = library->findBook("B001");
    Book *boo2 = library->findBook("B002");

    bool res1 = boo1->isAvailable();
    bool res2 = boo2->isAvailable();

    EXPECT_FALSE(res1);
    EXPECT_FALSE(res2);
}

/*
Test 3: ReturnOneBookThenBorrowAnother

    Alice borrows B001
    Alice returns B001
    Alice borrows B002
    Check B001 is available
    Check B002 is unavailable
    Check Alice's borrowed count is 1

*/

TEST_F(LibraryTest, ReturnOneBookThenBorrowAnother)
{
    library->borrowBook("M001", "B001");
    library->returnBook("M001", "B001");
    library->borrowBook("M001", "B002");

    Book* book1 = library->findBook("B001");
    Book* book2 = library->findBook("B002");

    bool res1 = book1->isAvailable();
    bool res2 = book2->isAvailable();

    EXPECT_TRUE(res1);
    EXPECT_FALSE(res2);

    Member * Mem = library->findMember("M001");
    int borrowedcount = Mem->getBorrowedCount();

    EXPECT_EQ(borrowedcount,1);
    
}